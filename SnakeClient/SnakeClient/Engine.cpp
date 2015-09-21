#include <stdafx.h>
#include "Engine.h"

Engine::Engine()
{
	currentTime = 0;
	timeForMove = 0;
	peer = socket_ptr(new ip::tcp::socket(service));
	MemoryBuffer::Init(20000);
	
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
		//if (currentTime >= timeForMove)
		//{
		//	boost::lock_guard<boost::mutex> lock(lpMutex);
		//	/*for (auto it = remotePlayers.begin(); it != remotePlayers.end(); it++)
		//	{
		//		field.ClearPlayer(it->OldBody());
		//		field.DrawPlayer(*it);
		//	}*/
		//	localPlayer->SaveBody();
		//	MovePlayer();
		//	field.ClearPlayer(localPlayer->OldBody());
		//	field.DrawPlayer(*localPlayer);
		//	
		//	canChangeDirection = true;
		//	timeForMove += SNAKE_MOVEMENT_DELTA_TIME;
		//}
		//if (gameState != EGameState::in_progress)
		//{
		//	field.PrintText("Game ended! Press Esc for exit...");
		//	Close();
		//	break;
		//}
		/*if (packetForSend.length()>0)
			SendData();*/
		currentTime += timer.elapsed() * 1000;
		boost::this_thread::sleep_for(boost::chrono::milliseconds(FIXED_UPDATE_DELTA_TIME));
		currentTime += FIXED_UPDATE_DELTA_TIME;
	}
}

EGameState Engine::GameState() const
{
	return gameState;
}

void Engine::MovePlayer()
{
	/*COORD newPos = localPlayer->HeadPosition();
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
	*/
	
}

void Engine::SetLocalDirection(EDirection direction)
{
	boost::lock_guard<boost::mutex> lock(lpMutex);
	packetForSend.push_back(EPacketType::client_info);
	packetForSend.push_back(1);
	packetForSend.push_back(direction);
	SendData();
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
	try
	{
		int* data = (int*)MemoryBuffer::Instance()->GetBuffer(packetForSend.size() * 4);
		for (size_t i = 0; i < packetForSend.size(); i++)
			data[i] = packetForSend[i];
		peer->send(buffer(data, packetForSend.size() * 4));
	}
	catch (boost::system::system_error& err){}
	packetForSend.clear();
}

void Engine::ReceiveData()
{
	unsigned char* v;
	size_t a = peer->available();
	v = new unsigned char[a];
	std::memset(v, 0, a);
	peer->receive(buffer(v, a));
	int* data = (int*)v;
	for (int i = 0; i < a / 4;)
	{
		int packet = data[i];
		i++;
		int length = data[i];
		length += ++i;
		switch ((EPacketType)packet)
		{
		case EPacketType::add_client:
		{
										int id = data[i];
										i++;
										std::vector<COORD> body;
										while (i < length)
										{
											COORD p;
											p.X = data[i];
											i++;
											p.Y = data[i];
											i++;
											body.push_back(p);
										}
										Player pl(id, body);
										remotePlayers.push_back(pl);
										field.DrawPlayer(pl);
		}
			break;
		case EPacketType::bonus_info:
		{
										bonusList.clear();
										while (i < length)
										{
											COORD p;
											p.X = data[i];
											i++;
											p.Y = data[i];
											i++;
											bonusList.push_back(p);
											field.DrawBonus(p);
										}
		}
			break;
		case EPacketType::client_info:
		{
										 int id = data[i];
										 i++;
										 Player* pl = nullptr;
										 if (localPlayer->Id() == id)
										 {
											 pl = localPlayer;
										 }
										 else
										 {
											 pl = GetPlayer(id);
										 }
										 if (pl)
										 {
											 std::vector<COORD> oldBody = pl->Body();
											 std::vector<COORD> forClear, forDraw;
											 pl->Body().clear();
											 std::vector<COORD> body;
											 while (i < length)
											 {
												 /*COORD begin;
												 begin.X = data[i];
												 i++;
												 begin.Y = data[i];
												 i++;
												 COORD end;
												 end.X = data[i];
												 i++;
												 end.Y = data[i];
												 if (i < length - 1)
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
													 if (!VectorContains(oldBody, body[j]))
														 forDraw.push_back(body[j]);
													 pl->Body().push_back(body[j]);
												 }*/
												 COORD p;
												 p.X = data[i];
												 i++;
												 p.Y = data[i];
												 i++;
												 if (!VectorContains(oldBody, p))
													 forDraw.push_back(p);
												 pl->Body().push_back(p);
											 }
											 for (auto it = oldBody.begin(); it != oldBody.end(); it++)
											 if (!VectorContains(pl->Body(), *it))
												 forClear.push_back(*it);
											 field.UpdatePlayer(forClear, forDraw, pl->IsLocal());
										 }

		}
			break;
		case EPacketType::start_info:
		{
										int error = data[i];
										i++;
										if ((EStartError)error== EStartError::max_players)
										{
											PrintString("The server filled. Press ESC to exit...");
										}
										else
										{
											int id = data[i];
											i++;
											std::vector<COORD> body;
											while(i<length)
											{
												COORD p;
												p.X = data[i];
												i++;
												p.Y = data[i];
												i++;
												body.push_back(p);
											}
											localPlayer = new LocalPlayer(id, body);
										}

		}
			break;
		case EPacketType::delete_player:
		{
										   int id = data[i];
										   i++;
										   if (localPlayer->Id() == id)
										   {
											   PrintString("You lose! Press ESC to exit...");
											   TerminateThread(serviceThread.native_handle(), 0);
										   }
										   else
										   {
											   Player* pl = GetPlayer(id);
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
		}
			break;
		case EPacketType::scores:
		{
									int current = data[i];
									i++;
									int max = data[i];
									i++;
									field.PrintScores(current, max);
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

void Engine::Close()
{
	packetForSend.push_back(EPacketType::delete_player);
	packetForSend.push_back(1);
	packetForSend.push_back(localPlayer->Id());
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