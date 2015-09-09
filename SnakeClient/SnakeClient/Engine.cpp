#include <stdafx.h>
#include "Engine.h"


Engine::Engine()
{
	currentTime = 0;
	timeForMove = 0;
	peer = socket_ptr(new ip::tcp::socket(service));
}

void Engine::Init()
{
	gameState = EGameState::in_progress;
	field.Init();
	Connect();
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
			break;
		}
		if (bytesForSend.length() > 0)
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
		bytesForSend = EatBonusSerialize(newPos);
	}
	else
	{
		localPlayer->Move();
		bytesForSend = localPlayer->Serialize();
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

std::string Engine::EatBonusSerialize(COORD& pos)
{
	std::string res;
	res += ToString((int)EPacketType::client_eat_bonus) + ",";
	res += ToString(localPlayer->Id()) + ",";
	res += ToString(pos.X) + "," + ToString(pos.Y) + ",";
	res += END_OF_PACKET;
	return res;
}

void Engine::SendData()
{
	peer->send(buffer(bytesForSend));
	bytesForSend.clear();
}

void Engine::ReceiveData()
{
	std::vector<char> v(peer->available());
	std::string tmp;
	peer->receive(buffer(v));
	std::string buf = VectorToString(v);
	while (buf.length() > 0)
	{
		size_t index = buf.find(END_OF_PACKET);
		tmp = buf.substr(0, index);
		int packet = std::atoi(tmp.substr(0, 1).c_str());
		std::vector<int> data = SplitStringToInt(tmp, ',');
		tmp.erase(0, 1);
		switch ((EPacketType)packet)
		{
		case EPacketType::add_client:
		{
										std::vector<COORD> body;
										auto it = data.begin() + 1;
										while (it != data.end())
										{
											COORD p;
											p.X = *it;
											it++;
											p.Y = *it;
											it++;
											body.push_back(p);
										}
										remotePlayers.push_back(Player(data[0], body));
		}
			break;
		case EPacketType::bonus_info:
		{
										for (auto it = data.begin(); it != data.end();)
										{
											COORD p;
											p.X = *it;
											it++;
											p.Y = *it;
											it++;
											bonusList.push_back(p);
										}
		}
			break;
		case EPacketType::client_eat_bonus:
		{
											  COORD p;
											  p.X = data[1];
											  p.Y = data[2];
											  Player* pl = GetPlayer(data[0]);
											  if (pl)
											  {
												  pl->AddBlock(p);
												  pl->drawRemoveTail = false;
											  }
		}
			break;
		case EPacketType::client_info:
		{
										 Player* pl = GetPlayer(data[0]);
										 if (pl)
											 pl->SetDirection((EDirection)data[1]);
		}
			break;
		case EPacketType::start_info:
		{
										std::vector<COORD> body;
										for (auto it = data.begin() + 1; it != data.end();)
										{
											COORD p;
											p.X = *it;
											it++;
											p.Y = *it;
											it++;
											body.push_back(p);
										}
										localPlayer = new LocalPlayer(data[0], body);
		}
			break;
		}
		buf.erase(0, index + 1);
	}
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