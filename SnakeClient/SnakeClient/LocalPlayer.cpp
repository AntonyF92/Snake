#include <stdafx.h>
#include "LocalPlayer.h"

LocalPlayer::LocalPlayer():Player::Player(true)
{
	for (int i = 0; i < INITIAL_SNAKE_LENGTH; i++)
	{
		SnakeBlock bl(i, 20, i + 10);
		body.push_back(bl);
		currentDirection = EDirection::up;
	}
}

EDirection LocalPlayer::CurrentDirection() const
{
	return currentDirection;
}

void LocalPlayer::SetDirection(EDirection direction)
{
	currentDirection = direction;
}

COORD LocalPlayer::HeadPosition() const
{
	return body[0].pos;
}