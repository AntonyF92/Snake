#include "stdafx.h"
#include "ServerApplication.h"
#include "GeneralDefinitions.h"



void ServerApplication::Init()
{
	addPlayerId = 0;
	ep = ip::tcp::endpoint(ip::tcp::v4(), SERVER_PORT);
	ip::tcp::acceptor acceptor(service, ep);
	serviceThread = boost::thread(boost::bind(&ServerApplication::Work, this));
	MemoryBuffer::Init();
	while (true)
	{
		socket_ptr peer(new ip::tcp::socket(service));
		acceptor.accept(*peer);
		Client cl(addPlayerId++, peer);
		CreateBody(cl);

		plMutex.lock();
		std::vector<NetPacket*> packets;
		int i = 0;
		for (auto it = players.begin(); it != players.end(); it++)
		{
			packets.push_back(it->SerializeFull());
			i++;
		}
		NetPacket* newPlayer = cl.SerializeFull();
		newPlayer->packet_id = EPacketType::start_info;
		size_t packetSize = 0;
		for (auto it = packets.begin(); it != packets.end(); it++)
			packetSize += (*it)->packetSize;
		packetSize += (newPlayer->packetSize + 4);
		TotalPacket* newPlayerPacket = (TotalPacket*)MemoryBuffer::Instance()->GetBuffer(packetSize);
		newPlayerPacket->packetCount = players.size() + 1;
		CopyPacket(*newPlayer, newPlayerPacket->packets[0]);
		int currentPosition = newPlayer->packetSize;
		for (int j = 0; j < players.size(); j++)
		{
			CopyPacket(*packets[j], newPlayerPacket->packets[currentPosition]);
			currentPosition += (*packets[j]).packetSize;
		}
		cl.Peer()->send(buffer(newPlayerPacket, packetSize));
		TotalPacket* packetForAll = (TotalPacket*)MemoryBuffer::Instance()->GetBuffer(newPlayer->packetSize + 4);
		newPlayer->packet_id = EPacketType::add_client;
		packetForAll->packetCount = 1;
		packetForAll->packets[0] = *newPlayer;
		SendToAll(packetForAll, newPlayer->packetSize + 4);
		players.push_back(cl);
		std::cout << "Add player: " << cl.Id() << std::endl;
		plMutex.unlock();
	}
}

void ServerApplication::Work()
{
	while (true)
	{
		plMutex.lock();
		CheckDisconnects();
		for (auto it = players.begin(); it != players.end(); it++)
			it->ReadData();
		plMutex.unlock();
		boost::this_thread::sleep_for(boost::chrono::milliseconds(FIXED_UPDATE_DELTA_TIME));
	}

}

void ServerApplication::SendToAll(TotalPacket* packet, size_t bytesCount)
{
	auto buff = buffer(packet, bytesCount);
	for (auto it = players.begin(); it != players.end(); it++)
	{
		if (it->Peer()->is_open())
			it->Peer()->send(buff);
	}
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

void ServerApplication::CheckDisconnects()
{
	for (auto it = players.begin(); it != players.end(); )
	{
		if (!it->Peer()->is_open())
		{
			TotalPacket* packet = (TotalPacket*)MemoryBuffer::Instance()->GetBuffer(20);
			packet->packetCount = 1;
			packet->packets[0].dataSize = 1;
			packet->packets[0].packetSize = 16;
			packet->packets[0].packet_id = EPacketType::delete_player;
			packet->packets[0].data[0] = it->Id();
			SendToAll(packet, 20);
			players.erase(it);
		}
		else
		{
			it++;
		}
	}
}