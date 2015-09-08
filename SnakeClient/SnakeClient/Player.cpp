#include <stdafx.h>
#include "Player.h"

Player::Player(bool is_local)
{
	local = is_local;
	drawRemoveTail = true;
}

bool Player::IsLocal() const
{
	return local;
}

const std::vector<SnakeBlock>& Player::Body()
{
	return body;
}

void Player::RemoveTail()
{
	body.erase(body.begin() + body.size() - 1);
}

void Player::Move(EDirection direction)
{
	for (int i = body.size() - 1; i > 0; i--)
	{
		body[i].pos.X = body[i - 1].pos.X;
		body[i].pos.Y = body[i - 1].pos.Y;
	}
	auto it = body.begin();
	switch (direction)
	{
	case EDirection::up: it->pos.Y--; break;
	case EDirection::down: it->pos.Y++; break;
	case EDirection::left: it->pos.X--; break;
	case EDirection::right: it->pos.X++; break;
	}
}

std::vector<SnakeBlock> Player::OldBody()
{
	return body;
}

void Player::AddBlock(SnakeBlock sb)
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
