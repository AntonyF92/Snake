#include <stdafx.h>
#include "LocalPlayer.h"

LocalPlayer::LocalPlayer():Player(true)
{
	for (int i = 0; i < INITIAL_SNAKE_LENGTH; i++)
	{
		SnakeBlock bl(i, 20, i + 10);
		body.push_back(bl);
		currentDirection = EDirection::up;
	}
}


COORD LocalPlayer::HeadPosition() const
{
	return body[0].pos;
}

bool LocalPlayer::CheckCollision(COORD& pos)
{
	for (auto it = body.begin() + 1; it != body.end(); it++)
	if ((it->pos.X == pos.X) && (it->pos.Y == pos.Y))
		return false;
	return true;
}

