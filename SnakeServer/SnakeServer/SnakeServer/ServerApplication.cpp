#include "stdafx.h"
#include "ServerApplication.h"
#include "GeneralDefinitions.h"



void ServerApplication::Init()
{
	std::srand(std::time(0));
	addPlayerId = 0;
	timeForMove = 0;
	currentTime = 0;
	timeForGenerateBonus = 0;
	ep = ip::tcp::endpoint(ip::tcp::v4(), SERVER_PORT);
	ip::tcp::acceptor acceptor(service, ep);
	serviceThread = boost::thread(boost::bind(&ServerApplication::Work, this));
	MemoryBuffer::Init();
	while (true)
	{
		socket_ptr peer(new ip::tcp::socket(service));
		acceptor.accept(*peer);
		peer->set_option(ip::tcp::no_delay(true));
		Client cl(addPlayerId++, peer);
		CreateBody(cl);

		plMutex.lock();
		std::string packetForNewPlayer = "";
		packetForNewPlayer += cl.SerializeFull(true);
		for (auto it = players.begin(); it != players.end(); it++)
		{
			if (!it->disconnected)
			{
				packetForNewPlayer += it->SerializeFull();
			}
		}
		packetForNewPlayer += SerializeBonuses();
		cl.Peer()->send(buffer(packetForNewPlayer));
		SendToAll(cl.SerializeFull());
		players.push_back(cl);
		std::cout << "Add player: " << cl.Id() << std::endl;
		plMutex.unlock();
	}
}

void ServerApplication::Work()
{
	while (true)
	{
		boost::timer timer;
		plMutex.lock();
		std::string packet = "";
		packet += CheckDisconnects();
		for (auto it = players.begin(); it != players.end(); it++)
		{
			it->ReadData();
		}
		if (currentTime >= timeForMove)
		{
			for (auto it = players.begin(); it != players.end(); it++)
			{
				packet += MovePlayer(*it);				
			}
			timeForMove += SNAKE_MOVEMENT_DELTA_TIME;
		}
		if (currentTime >= timeForGenerateBonus&&bonusList.size()<MAX_BONUS_COUNT)
		{
			GenerateBonuses();
			packet += SerializeBonuses();
		}
		if (packet.length() > 0)
			SendToAll(packet);
		plMutex.unlock();
		currentTime += timer.elapsed() * 1000;
		boost::this_thread::sleep_for(boost::chrono::milliseconds(FIXED_UPDATE_DELTA_TIME));
		currentTime += FIXED_UPDATE_DELTA_TIME;
	}

}

void ServerApplication::SendToAll(std::string packet)
{
	auto buff = buffer(packet);
	for (auto it = players.begin(); it != players.end(); it++)
	{
		try
		{
			if (!it->disconnected)
				it->Peer()->send(buff);
		}
		catch(boost::system::system_error& err)
		{
			it->disconnected = true;
		}
	}
	packet = "";
}

void ServerApplication::CreateBody(Client& cl)
{
	for (int i = 0; i < SNAKE_INIT_LENGTH; i++)
	{
		COORD p;
		p.X = 20;
		p.Y = 10 + i;
		cl.Body().push_back(p);
	}
	
}

void ServerApplication::CopyPacket(NetPacket& in, NetPacket& out)
{
	out.dataSize = in.dataSize;
	out.packetSize = in.packetSize;
	out.packet_id = in.packet_id;
	for (int i = 0; i < in.dataSize; i++)
		out.data[i] = in.data[i];
}

std::string ServerApplication::CheckDisconnects()
{
	std::string packet = "";
	for (auto it = players.begin(); it != players.end();)
	{
		if (it->disconnected || it->finished)
		{
			if (it->disconnected)
				packet += it->SerializeDelete();
			std::cout << "Remove player " << it->Id() << std::endl;
			it = players.erase(it);
		}
		else
			it++;
	}
	return packet;
}

std::string ServerApplication::MovePlayer(Client& cl)
{
	std::string res = "";
	COORD newPos = cl.HeadPosition();
	switch (cl.CurrentDirection())
	{
	case EDirection::up: newPos.Y--; break;
	case EDirection::down: newPos.Y++; break;
	case EDirection::left: newPos.X--; break;
	case EDirection::right: newPos.X++; break;
	}
	if (!CheckHeadPos(newPos) || !cl.CheckCollision(newPos))
	{
		res = cl.SerializeDelete();
		cl.finished = true;
	}
	else if (CheckPosForBonus(newPos))
	{
		cl.AddBlock(newPos);
		res = cl.Serialize();
		RemoveBonus(newPos);
		timeForGenerateBonus = currentTime + GENERATE_BONUS_DELTA_TIME;
	}
	else
	{
		cl.Move();
		res = cl.Serialize();
	}
	cl.directionChanged = false;
	return res;
}

bool ServerApplication::CheckHeadPos(COORD& pos)
{
	return ((pos.X != 0) && (pos.X != GAME_FIELD_WIDTH) && (pos.Y != 0) && (pos.Y != GAME_FIELD_HEIGHT));
}

bool ServerApplication::CheckPosForBonus(COORD& pos)
{
	for (auto it = bonusList.begin(); it != bonusList.end(); it++)
	if (it->X == pos.X && it->Y == pos.Y)
		return true;
	return false;
}

void ServerApplication::GenerateBonuses()
{
	std::vector<COORD> allPlayers;
	for (auto pl = players.begin(); pl != players.end(); pl++)
	{
		for (auto it = pl->Body().begin(); it != pl->Body().end(); it++)
			allPlayers.push_back(*it);
	}
	while (bonusList.size() < MAX_BONUS_COUNT)
	{
		COORD p;
		do
		{			
			p.X = 1 + std::rand() % (GAME_FIELD_WIDTH - 1);
			p.Y = 1 + std::rand() % (GAME_FIELD_HEIGHT - 1);
		} while (VectorContains(allPlayers, p));
		bonusList.push_back(p);
	}
}

std::string ServerApplication::SerializeBonuses()
{
	std::string res = "";
	res += ToString(EPacketType::bonus_info) + ",";
	for (auto it = bonusList.begin(); it != bonusList.end(); it++)
	{
		res += ToString(it->X) + ",";
		res += ToString(it->Y) + ",";
	}
	res += END_OF_PACKET;
	return res;
}

void ServerApplication::RemoveBonus(COORD& pos)
{
	for (auto it = bonusList.begin(); it != bonusList.end(); it++)
	{
		if (it->X == pos.X&&it->Y == pos.Y)
		{
			bonusList.erase(it);
			break;
		}
	}
}