#include "stdafx.h"
#include "Client.h"


Client::Client(int id, socket_ptr peer)
{
	this->id = id;
	this->peer = peer;
	justAdded = true;
	memBuffer = new unsigned char[65536];
	currentDirection = EDirection::up;
}

void Client::ReadData()
{
	if (!peer->available())
		return;
	unsigned char* v;
	size_t a = peer->available();
	v = new unsigned char[a];
	std::memset(v, 0, a);
	peer->receive(buffer(v, a));
	NetPacket* packet = (NetPacket*)v;
	switch ((EPacketType)packet->packet_id)
	{
	case EPacketType::client_eat_bonus:
	{

	}
		break;
	case EPacketType::client_info:
	{
									 currentDirection = (EDirection)packet->data[1];
									 body.clear();
									 for (int i = 2; i < packet->dataSize;)
									 {
										 COORD p;
										 p.X = packet->data[i];
										 i++;
										 p.Y = packet->data[i];
										 i++;
										 body.push_back(p);
									 }
	}
		break;
	}
	delete v;
}


int Client::Id()
{
	return id;
}

bool Client::DirectionChanged()
{
	return directionChanged;
}

EDirection Client::CurrentDirection()
{
	return currentDirection;
}

NetPacket Client::Serialize()
{
	NetPacket packet;
	packet.dataSize = 2;
	packet.packet_id = EPacketType::client_info;
	packet.packetSize = 20;
	packet.data[0] = id;
	packet.data[1] = currentDirection;
	return packet;
}

NetPacket* Client::SerializeFull()
{
	size_t size = 20 + 4 * body.size() * 2;
	NetPacket* packet = (NetPacket*)memBuffer;
	packet->packet_id = EPacketType::add_client;
	packet->dataSize = body.size()*2 + 2;
	packet->data[0] = id;
	packet->data[1] = currentDirection;
	packet->packetSize = size;
	int i = 2;
	for (size_t j=0;j<body.size();j++)
	{
		auto coord = body[j];
		packet->data[i] = coord.X;
		i++;  
		packet->data[i] = coord.Y;
		i++;
	}
	return packet;
}

socket_ptr Client::Peer()
{
	return peer;
}

std::vector<COORD>& Client::Body()
{
	return body;
}