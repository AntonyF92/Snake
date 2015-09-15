#include "stdafx.h"
#include "Client.h"


Client::Client(int id, socket_ptr peer)
{
	this->id = id;
	this->peer = peer;
	disconnected = false;
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
				case EPacketType::client_eat_bonus:
				{
													  COORD p;
													  p.X = packet[2];
													  p.Y = packet[3];
													  body.insert(body.begin(), p);
													  res += data.substr(0, data.find(END_OF_PACKET) + 1);
				}
					break;
				case EPacketType::client_info:
				{
												 body.clear();
												 for (int i = 2; i < packet.size();)
												 {
													 COORD p;
													 p.X = packet[i];
													 i++;
													 p.Y = packet[i];
													 i++;
													 body.push_back(p);
												 }
												 res += Serialize();
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

bool Client::DirectionChanged()
{
	return directionChanged;
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