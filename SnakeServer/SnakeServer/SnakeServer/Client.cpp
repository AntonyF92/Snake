#include "stdafx.h"
#include "Client.h"


Client::Client(int id, socket_ptr peer)
{
	this->id = id;
	this->peer = peer;
}

void Client::ReadData()
{

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
	}
	delete v;
}


int Client::Id()
{
	return id;
}