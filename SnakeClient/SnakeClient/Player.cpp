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

std::string Player::Serialize()
{
	std::string res;
	res += ToString((int)EPacketType::client_info) + ",";
	res += ToString(id) + ",";
	res += ToString((int)currentDirection) + ",";
	for (auto it = body.begin(); it != body.end(); it++)
	{
		res += ToString(it->X) + "," + ToString(it->Y) + ",";
	}
	res += END_OF_PACKET;
	return res;
}

int Player::Id()
{
	return id;
}
