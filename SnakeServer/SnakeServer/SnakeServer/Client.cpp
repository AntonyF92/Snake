#include "stdafx.h"
#include "Client.h"


Client::Client(int id, socket_ptr peer)
{
	this->id = id;
	this->peer = peer;
	disconnected = false;
	finished = false;
	currentDirection = EDirection::up;
	scores = 0;
}

void Client::ReadData()
{
	
	if (!peer->available())
		return;
	unsigned char* v;
	try
	{
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
			i++;
				switch ((EPacketType)packet)
				{
				case EPacketType::client_info:
				{
												 EDirection direction = (EDirection)data[i];
												 i++;
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
	}
	catch (boost::system::system_error& err)
	{
		disconnected = true;
	}
	delete v;
}


int Client::Id()
{
	return id;
}

EDirection Client::CurrentDirection()
{
	return currentDirection;
}

std::vector<int> Client::Serialize()
{
	std::vector<int> res;
	res.push_back(EPacketType::client_info);
	res.push_back(body.size()*2 + 1);
	res.push_back(id);
	/*res.push_back(body.begin()->X);
	res.push_back(body.begin()->Y);
	COORD last = *(body.begin() + 1);
	bool x = last.X == body.begin()->X;
	bool y = last.Y == body.begin()->Y;
	
	for (auto it = body.begin() + 2; it != body.end(); it++)
	{
		if ((last.X == it->X&&!x) || (last.Y == it->Y&&!y))
		{
			res.push_back(last.X);
			res.push_back(last.Y);
			x = last.X == it->X;
			y = last.Y = it->Y;
		}
		last = *it;
	}
	res.push_back(body.rbegin()->X);
	res.push_back(body.rbegin()->Y);
	int length = res.size() - 1;
	res.insert(res.begin() + 1, length);*/

	for (auto it = body.begin(); it != body.end(); it++)
	{
		res.push_back(it->X);
		res.push_back(it->Y);
	}
	return res;
}

std::vector<int> Client::SerializeFull(bool for_start)
{
	std::vector<int> res;
	if (for_start)
	{
		res.push_back(EPacketType::start_info);
		res.push_back(body.size() * 2 + 2);
		res.push_back(EStartError::no);
	}
	else
	{
		res.push_back(EPacketType::add_client);
		res.push_back(body.size() * 2 + 1);
	}
	res.push_back(id);
	for (auto it = body.begin(); it != body.end(); it++)
	{
		res.push_back(it->X);
		res.push_back(it->Y);
	}
	return res;
}

socket_ptr Client::Peer()
{
	return peer;
}

std::vector<COORD>& Client::Body()
{
	return body;
}

std::vector<int> Client::SerializeDelete()
{
	std::vector<int> res;
	res.push_back(EPacketType::delete_player);
	res.push_back(1);
	res.push_back(id);
	return res;
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