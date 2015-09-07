#include <stdafx.h>
#include "Player.h"

Player::Player(bool is_local)
{
	local = is_local;
}

bool Player::IsLocal() const
{
	return local;
}

std::vector<SnakeBlock> Player::Body() const
{
	return body;
}