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
	maxScores = 0;
	bonusLocked = false;
	scoreChanged = false;
	ep = ip::tcp::endpoint(ip::tcp::v4(), SERVER_PORT);
	ip::tcp::acceptor acceptor(service, ep);
	serviceThread = boost::thread(boost::bind(&ServerApplication::Work, this));
	MemoryBuffer::Init();
	while (true)
	{
		socket_ptr peer(new ip::tcp::socket(service));
		acceptor.accept(*peer);
		std::string packetForNewPlayer = "";
		plMutex.lock();
		if (players.size() == MAX_PLAYER_COUNT)
		{
			packetForNewPlayer += ToString(EPacketType::start_info) + ",";
			packetForNewPlayer += ToString(EStartError::max_players) + "," + END_OF_PACKET;
			peer->send(buffer(packetForNewPlayer));
			peer->close();
		}
		else
		{
			peer->set_option(ip::tcp::no_delay(true));
			Client cl(addPlayerId++, peer);
			CreateBody(cl);

			packetForNewPlayer += cl.SerializeFull(true);
			for (auto it = players.begin(); it != players.end(); it++)
			{
				if (!it->disconnected)
				{
					packetForNewPlayer += it->SerializeFull();
				}
			}
			packetForNewPlayer += SerializeBonuses();
			packetForNewPlayer += SerializeScoresForPlayer(cl);
			cl.Peer()->send(buffer(packetForNewPlayer));
			SendToAll(cl.SerializeFull());
			players.push_back(cl);
			std::cout << "Add player: " << cl.Id() << std::endl;
		}
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
			bonusLocked = false;
		}
		if (packet.length() > 0)
			SendToAll(packet);
		if (scoreChanged)
			SendScores();
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
		cl.scores++;
		maxScores = cl.scores > maxScores ? cl.scores : maxScores;
		scoreChanged = true;
		if (!bonusLocked)
		{
			timeForGenerateBonus = currentTime + GENERATE_BONUS_DELTA_TIME;
			bonusLocked = true;
		}
		std::cout << "Player " << cl.Id() << " eat bonus" << std::endl;
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
	return ((pos.X != BORDER_X) && (pos.X != GAME_FIELD_WIDTH) && (pos.Y != BORDER_Y) && (pos.Y != GAME_FIELD_HEIGHT));
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
			p.X = BORDER_X + 1 + std::rand() % (GAME_FIELD_WIDTH - 1 - BORDER_X);
			p.Y = BORDER_Y + 1 + std::rand() % (GAME_FIELD_HEIGHT - 1 - BORDER_Y);
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

void ServerApplication::SendScores()
{
	for (auto it = players.begin(); it != players.end(); it++)
	{
		auto packet = SerializeScoresForPlayer(*it);
		it->Peer()->send(buffer(packet));
	}
	scoreChanged = false;
}

std::string ServerApplication::SerializeScoresForPlayer(Client& cl)
{
	std::string packet = "";
	packet += ToString(EPacketType::scores) + ",";
	packet += ToString(cl.scores) + ",";
	packet += ToString(maxScores) + ",";
	packet += END_OF_PACKET;
	return packet;
}