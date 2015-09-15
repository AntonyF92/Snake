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
		plMutex.lock();
		std::string packet = "";
		packet += CheckDisconnects();
		for (auto it = players.begin(); it != players.end(); it++)
		{
			std::string s = it->ReadData();
			if (s.length() > 0)
				packet += s;
		}
		if (packet.length() > 0)
			SendToAll(packet);
		plMutex.unlock();
		boost::this_thread::sleep_for(boost::chrono::milliseconds(FIXED_UPDATE_DELTA_TIME));
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
		if (it->disconnected)
		{
			packet += it->SerializeDelete();
			std::cout << "Remove player " << it->Id() << std::endl;
			it = players.erase(it);
		}
		else
			it++;
	}
	return packet;
}