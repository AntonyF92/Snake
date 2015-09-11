#include <stdafx.h>
#include "Engine.h"

Engine::Engine()
{
	currentTime = 0;
	timeForMove = 0;
	peer = socket_ptr(new ip::tcp::socket(service));
	MemoryBuffer::Init();
	packetForSend = nullptr;
}

void Engine::Init()
{
	gameState = EGameState::in_progress;
	field.Init();
	Connect();
	while (!peer->available())
		boost::this_thread::sleep_for(boost::chrono::milliseconds(FIXED_UPDATE_DELTA_TIME));
	ReceiveData();
	canChangeDirection = true;
	field.DrawPlayer(*localPlayer);
	serviceThread = boost::thread(boost::bind(&Engine::FixedUpdate, this));
}

void Engine::Connect()
{
	ip::tcp::endpoint ep(ip::address::from_string(SERVER_IP), SERVER_PORT);
	peer->connect(ep);
}

void Engine::FixedUpdate()
{
	while (true)
	{
		boost::timer timer;
		if (peer->available())
			ReceiveData();
		if (currentTime >= timeForMove)
		{
			boost::lock_guard<boost::mutex> lock(lpMutex);
			auto old = localPlayer->OldBody();
			MovePlayer();
			field.ClearPlayer(old);
			field.DrawPlayer(*localPlayer);
			for (auto it = remotePlayers.begin(); it != remotePlayers.end(); it++)
			{
				old = it->OldBody();
				it->Move();
				field.ClearPlayer(old);
				field.DrawPlayer(*it);
			}
			canChangeDirection = true;
			timeForMove += SNAKE_MOVEMENT_DELTA_TIME;
		}
		if (gameState != EGameState::in_progress)
		{
			field.PrintText("Game ended! Press Esc for exit...");
			peer->shutdown(socket_base::shutdown_both);
			break;
		}
		if (packetForSend)
			SendData();
		currentTime += timer.elapsed() * 1000;
		boost::this_thread::sleep_for(boost::chrono::milliseconds(FIXED_UPDATE_DELTA_TIME));
		currentTime += FIXED_UPDATE_DELTA_TIME;
	}
	TerminateThread(serviceThread.native_handle(), 0);
}

EGameState Engine::GameState() const
{
	return gameState;
}

void Engine::MovePlayer()
{
	COORD newPos = localPlayer->HeadPosition();
	switch (localPlayer->CurrentDirection())
	{
	case EDirection::up: newPos.Y--; break;
	case EDirection::down: newPos.Y++; break;
	case EDirection::left: newPos.X--; break;
	case EDirection::right: newPos.X++; break;
	}
	if (Engine::CheckPosForBonus(newPos))
	{
		localPlayer->AddBlock(newPos);
		packetForSend = EatBonusSerialize(newPos);
	}
	else
	{
		localPlayer->Move();
		packetForSend = localPlayer->Serialize();
	}
	if (!field.CheckHeadPos(localPlayer->HeadPosition()) || !localPlayer->CheckCollision(localPlayer->HeadPosition()))
		gameState = EGameState::lose;
	
}

void Engine::SetLocalDirection(EDirection direction)
{
	boost::lock_guard<boost::mutex> lock(lpMutex);
	if (canChangeDirection)
	{
		canChangeDirection = !localPlayer->SetDirection(direction);
	}
}

bool Engine::CheckPosForBonus(COORD& pos)
{
	for (auto it = bonusList.begin();it!=bonusList.end();it++)
	if (it->X == pos.X && it->Y == pos.Y)
		return true;
	return false;
}

NetPacket* Engine::EatBonusSerialize(COORD& pos)
{
	size_t size = 24;
	NetPacket* packet = (NetPacket*)MemoryBuffer::Instance()->GetBuffer(size);
	packet->dataSize = 3;
	packet->packet_id = EPacketType::client_eat_bonus;
	packet->data[0] = localPlayer->Id();
	packet->data[1] = pos.X;
	packet->data[2] = pos.Y;
	packet->packetSize = size;
	return packet;
}

void Engine::SendData()
{
	peer->send(buffer(packetForSend, packetForSend->packetSize));
	packetForSend = nullptr;
}

void Engine::ReceiveData()
{
	unsigned char* v;
	size_t a = peer->available();
	v = new unsigned char[a];
	std::memset(v, 0, a);
	peer->receive(buffer(v, a));
	TotalPacket* totalPacket = (TotalPacket*)v;
	for (int i = 0; i < totalPacket->packetCount; i++)
	{
		NetPacket* packet = &totalPacket->packets[i];
		switch ((EPacketType)packet->packet_id)
		{
		case EPacketType::add_client:
		{
										std::vector<COORD> body;
										for (int i = 2; i < packet->dataSize;)
										{
											COORD p;
											p.X = packet->data[i];
											i++;
											p.Y = packet->data[i];
											i++;
											body.push_back(p);
										}
										Player pl(packet->data[0], body);
										pl.SetDirection((EDirection)packet->data[1]);
										remotePlayers.push_back(pl);
		}
			break;
		case EPacketType::bonus_info:
		{
										for (int i = 0; i < packet->dataSize;)
										{
											COORD p;
											p.X = packet->data[i];
											i++;
											p.Y = packet->data[i];
											i++;
											bonusList.push_back(p);
										}
		}
			break;
		case EPacketType::client_eat_bonus:
		{
											  COORD p;
											  p.X = packet->data[1];
											  p.Y = packet->data[2];
											  Player* pl = GetPlayer(packet->data[0]);
											  if (pl)
											  {
												  pl->AddBlock(p);
												  pl->drawRemoveTail = false;
											  }
		}
			break;
		case EPacketType::client_info:
		{
										 Player* pl = GetPlayer(packet->data[0]);
										 if (pl)
											 pl->SetDirection((EDirection)packet->data[1]);
		}
			break;
		case EPacketType::start_info:
		{
										std::vector<COORD> body;
										for (int i = 2; i < packet->dataSize;)
										{
											COORD p;
											p.X = packet->data[i];
											i++;
											p.Y = packet->data[i];
											i++;
											body.push_back(p);
										}
										localPlayer = new LocalPlayer(packet->data[0], body);
										localPlayer->SetDirection((EDirection)packet->data[1]);
		}
			break;
		case EPacketType::delete_player:
		{
										   Player* pl = GetPlayer(packet->data[0]);
										   if (pl)
										   {
											   field.ClearPlayer(pl->OldBody());
											   for (auto it = remotePlayers.begin(); it != remotePlayers.end(); it++)
											   {
												   if (it->Id() == pl->Id())
												   {
													   remotePlayers.erase(it);
													   break;
												   }
											   }
										   }
		}
			break;
		}
	}
	delete v;
}

Player* Engine::GetPlayer(int id)
{
	for (auto it = remotePlayers.begin(); it != remotePlayers.end(); it++)
	{
		if (it->Id() == id)
			return &*it;
	}
	return nullptr;
}

void Engine::PrintString(const char* str)
{
	field.PrintText(str);
}