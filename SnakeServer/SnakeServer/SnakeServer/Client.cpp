#include "stdafx.h"
#include "Client.h"


Client::Client(int id, socket_ptr peer)
{
	this->id = id;
	this->peer = peer;
	disconnected = false;
	finished = false;
	currentDirection = EDirection::up;
}

std::string Client::ReadData()
{
	
	if (!peer->available())
		return "";
	std::string res = "";
	char* v;
	try
	{
		size_t a = peer->available();
		v = new char[a];
		std::memset(v, 0, a);
		peer->receive(buffer(v, a));
		std::string data(v);
		FormattPacket(data);
		while (data.length() > 0)
		{
			std::vector<int> packet = SplitStringToInt(data.substr(0, data.find(END_OF_PACKET)), ',');
			if (packet.size())
			{
				switch ((EPacketType)packet[0])
				{
				case EPacketType::client_info:
				{
												 EDirection direction = (EDirection)packet[1];
												 if (((currentDirection ^ direction) == 2) || ((currentDirection ^ direction) == 6) || (currentDirection == direction) || directionChanged)
													 break;
												 currentDirection = direction;
												 std::cout << "Player " << id << " change move direction to " << direction << std::endl;
												 directionChanged = true;
				}
					break;
				case EPacketType::delete_player:
				{
												   disconnected = true;
				}
					break;
				}
			}
			data = data.erase(0, data.find('|') + 1);
		}
	}
	catch (boost::system::system_error& err)
	{
		disconnected = true;
	}
	delete v;
	return res;
}


int Client::Id()
{
	return id;
}

EDirection Client::CurrentDirection()
{
	return currentDirection;
}

std::string Client::Serialize()
{
	std::string packet = "";
	packet += ToString((int)EPacketType::client_info) + ",";
	packet += ToString(id) + ",";
	/*COORD last = *body.begin();
	packet += ToString(last.X) + ",";
	packet += ToString(last.Y) + ",";
	for (auto it = body.begin(); it != body.end(); it++)
	{
		if ((last.X != it->X) && (last.Y != it->Y))
		{
			packet += ToString(it->X) + ",";
			packet += ToString(it->Y) + ",";
		}
		last = *it;
	}
	packet += ToString(body.rbegin()->X) + ",";
	packet += ToString(body.rbegin()->Y) + ",";*/

	for (size_t j = 0; j<body.size(); j++)
	{
		auto coord = body[j];
		packet += ToString(coord.X) + ",";
		packet += ToString(coord.Y) + ",";
	}

	packet += END_OF_PACKET;
	return packet;
}

std::string Client::SerializeFull(bool for_start)
{
	std::string packet = "";
	if (for_start)
		packet += ToString((int)EPacketType::start_info) + ",";
	else
		packet += ToString((int)EPacketType::add_client) + ",";
	packet += ToString(id) + ",";
	packet += ToString((int)currentDirection) + ",";
	for (size_t j=0;j<body.size();j++)
	{
		auto coord = body[j];
		packet += ToString(coord.X) + ",";
		packet += ToString(coord.Y) + ",";
	}
	packet += END_OF_PACKET;
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

std::string Client::SerializeDelete()
{
	std::string packet = "";
	packet += ToString((int)EPacketType::delete_player) + ",";
	packet += ToString(id) + ",";
	packet += END_OF_PACKET;
	return packet;
}

void Client::SetDirection(EDirection direction)
{
	if (((currentDirection ^ direction) == 2) || ((currentDirection ^ direction) == 6) || (currentDirection == direction) || directionChanged)
		return;
	currentDirection = direction;
	directionChanged = true;
}

void Client::Move()
{
	for (int i = body.size() - 1; i > 0; i--)
	{
		body[i].X = body[i - 1].X;
		body[i].Y = body[i - 1].Y;
	}
	auto it = body.begin();
	switch (currentDirection)
	{
	case EDirection::up: it->Y--; break;
	case EDirection::down: it->Y++; break;
	case EDirection::left: it->X--; break;
	case EDirection::right: it->X++; break;
	}


}

COORD Client::HeadPosition() const
{
	return *body.begin();
}

void Client::AddBlock(COORD sb)
{
	body.insert(body.begin(), sb);
}

bool Client::CheckCollision(COORD& pos)
{
	for (auto it = body.begin() + 1; it != body.end(); it++)
	if ((it->X == pos.X) && (it->Y == pos.Y))
		return false;
	return true;
}