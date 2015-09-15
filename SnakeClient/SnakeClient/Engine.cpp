#include <stdafx.h>
#include "Engine.h"

Engine::Engine()
{
	currentTime = 0;
	timeForMove = 0;
	peer = socket_ptr(new ip::tcp::socket(service));
	MemoryBuffer::Init();
	packetForSend = "";
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
		bool save = false;
		if (peer->available())
		{
			//for (auto it = remotePlayers.begin(); it != remotePlayers.end(); it++)
			//	it->SaveBody();
			//save = true;
			ReceiveData();
		}
		if (currentTime >= timeForMove)
		{
			boost::lock_guard<boost::mutex> lock(lpMutex);
			/*for (auto it = remotePlayers.begin(); it != remotePlayers.end(); it++)
			{
				field.ClearPlayer(it->OldBody());
				field.DrawPlayer(*it);
			}*/
			localPlayer->SaveBody();
			MovePlayer();
			field.ClearPlayer(localPlayer->OldBody());
			field.DrawPlayer(*localPlayer);
			
			canChangeDirection = true;
			timeForMove += SNAKE_MOVEMENT_DELTA_TIME;
		}
		if (gameState != EGameState::in_progress)
		{
			field.PrintText("Game ended! Press Esc for exit...");
			Close();
			break;
		}
		if (packetForSend.length()>0)
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
	if (!field.CheckHeadPos(newPos) || !localPlayer->CheckCollision(newPos))
		gameState = EGameState::lose;
	else if (Engine::CheckPosForBonus(newPos))
	{
		localPlayer->AddBlock(newPos);
		packetForSend = EatBonusSerialize(newPos);
	}
	else
	{
		localPlayer->Move();
		packetForSend = localPlayer->Serialize();
	}
	
	
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

std::string Engine::EatBonusSerialize(COORD& pos)
{
	std::string packet = "";
	packet += ToString((int)EPacketType::client_eat_bonus) + ",";
	packet += ToString(localPlayer->Id()) + ",";
	packet += ToString(pos.X) + ",";
	packet += ToString(pos.Y) + ",";
	packet += END_OF_PACKET;
	return packet;
}

void Engine::SendData()
{
	peer->send(buffer(packetForSend));
	packetForSend = "";
}

void Engine::ReceiveData()
{
	char* v;
	size_t a = peer->available();
	v = new char[a];
	std::memset(v, 0, a);
	peer->receive(buffer(v, a));
	std::string data(v);
	FormattPacket(data);
	while (data.length()>0)
	{
		std::vector<int> packet = SplitStringToInt(data.substr(0, data.find('|')), ',');
		if (packet.size())
		{
			switch ((EPacketType)packet[0])
			{
			case EPacketType::add_client:
			{
											std::vector<COORD> body;
											for (size_t i = 3; i < packet.size();)
											{
												COORD p;
												p.X = packet[i];
												i++;
												p.Y = packet[i];
												i++;
												body.push_back(p);
											}
											Player pl(packet[1], body);
											remotePlayers.push_back(pl);
											field.DrawPlayer(pl);
			}
				break;
			case EPacketType::bonus_info:
			{
											for (size_t i = 1; i < packet.size();)
											{
												COORD p;
												p.X = packet[i];
												i++;
												p.Y = packet[i];
												i++;
												bonusList.push_back(p);
											}
			}
				break;
			case EPacketType::client_eat_bonus:
			{
												  COORD p;
												  p.X = packet[2];
												  p.Y = packet[3];
												  Player* pl = GetPlayer(packet[1]);
												  if (pl)
												  {
													  pl->AddBlock(p);
													  pl->drawRemoveTail = false;
												  }
			}
				break;
			case EPacketType::client_info:
			{
											 Player* pl = GetPlayer(packet[1]);
											 if (pl)
											 {
												 field.ClearPlayer(pl->Body());
												 pl->Body().clear();
												 std::vector<COORD> body;
												 for (size_t i = 2; i < packet.size();)
												 {
													 /*COORD begin;
													 begin.X = packet[i];
													 i++;
													 begin.Y = packet[i];
													 i++;
													 COORD end;
													 end.X = packet[i];
													 i++;
													 end.Y = packet[i];
													 if (i < packet.size() - 1)
														 i--;
													 else
													 {
														 i++;
													 }
													 body = CalcBody(begin, end);
													 if (pl->Body().size()>0)
													 {
														 if ((pl->Body().rbegin()->X == body.begin()->X) && (pl->Body().rbegin()->Y == body.begin()->Y))
															 body.erase(body.begin());
													 }
													 for (size_t j = 0; j < body.size(); j++)
													 {
														 pl->Body().push_back(body[j]);
													 }*/
													 COORD p;
													 p.X = packet[i];
													 i++;
													 p.Y = packet[i];
													 i++;
													 pl->Body().push_back(p);
												 }
												 field.DrawPlayer(*pl);
											 }
			}
				break;
			case EPacketType::start_info:
			{
											std::vector<COORD> body;
											for (size_t i = 3; i < packet.size();)
											{
												COORD p;
												p.X = packet[i];
												i++;
												p.Y = packet[i];
												i++;
												body.push_back(p);
											}
											localPlayer = new LocalPlayer(packet[1], body);
											localPlayer->SetDirection((EDirection)packet[2]);
			}
				break;
			case EPacketType::delete_player:
			{
											   Player* pl = GetPlayer(packet[1]);
											   if (pl)
											   {
												   field.ClearPlayer(pl->Body());
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
		data = data.erase(0, data.find('|') + 1);
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

void Engine::Close()
{
	packetForSend += ToString((int)EPacketType::delete_player) + ",";
	packetForSend += ToString(localPlayer->Id()) + ",";
	packetForSend += END_OF_PACKET;
	SendData();
	peer->close();
}

std::vector<COORD> Engine::CalcBody(COORD begin, COORD end)
{
	std::vector<COORD> res;
	res.push_back(begin);
	
	COORD last = begin;
	while ((last.X != end.X) || (last.Y != end.Y))
	{
		COORD newPos;
		if (last.X == end.X)
		{
			newPos.X = last.X;
			if (last.Y < end.Y)
				newPos.Y = last.Y + 1;
			else
			{
				newPos.Y = last.Y - 1;
			}
		}
		else if (last.Y == end.Y)
		{
			newPos.Y = last.Y;
			if (last.X < end.X)
				newPos.X = last.X + 1;
			else
			{
				newPos.X = last.X - 1;
			}
		}
		res.push_back(newPos);
		last = newPos;
	}
	return res;
}