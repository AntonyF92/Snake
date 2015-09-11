#include <stdafx.h>
#include "LocalPlayer.h"

LocalPlayer::LocalPlayer(int id, std::vector<COORD> body):Player(id, body)
{
	local = true;
	this->body = body;
}


COORD LocalPlayer::HeadPosition() const
{
	return body[0];
}

bool LocalPlayer::CheckCollision(COORD& pos)
{
	for (auto it = body.begin() + 1; it != body.end(); it++)
	if ((it->X == pos.X) && (it->Y == pos.Y))
		return false;
	return true;
}

