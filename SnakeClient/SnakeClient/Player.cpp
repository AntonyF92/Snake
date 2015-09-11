#include <stdafx.h>
#include "Player.h"
#include <string>


Player::Player(int id, std::vector<COORD> body)
{
	this->id = id;
	drawRemoveTail = true;
	this->body = body;
	local = false;
}

bool Player::IsLocal() const
{
	return local;
}


const std::vector<COORD>& Player::Body()
{
	return body;
}

void Player::RemoveTail()
{
	body.erase(body.begin() + body.size() - 1);
}

void Player::Move()
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

std::vector<COORD> Player::OldBody()
{
	return body;
}

void Player::AddBlock(COORD sb)
{
	body.insert(body.begin(), sb);
}

EDirection Player::CurrentDirection() const
{
	return currentDirection;
}

bool Player::SetDirection(EDirection direction)
{
	if (((currentDirection ^ direction) == 2) || ((currentDirection ^ direction) == 6) || (currentDirection == direction))
		return false;
	currentDirection = direction;
	return true;
}

NetPacket* Player::Serialize()
{
	size_t size = 20 + 4 * body.size() * 2;
	NetPacket* packet = (NetPacket*)MemoryBuffer::Instance()->GetBuffer(size);
	packet->packet_id = EPacketType::client_info;
	packet->dataSize = body.size()*2 + 2;
	packet->data[0] = id;
	packet->data[1] = currentDirection;
	packet->packetSize = size;
	int i = 2;
	for (auto it = body.begin(); it != body.end(); it++)
	{
		packet->data[i] = it->X;
		i++;
		packet->data[i] = it->Y;
		i++;
	}
	return packet;
}

int Player::Id()
{
	return id;
}
