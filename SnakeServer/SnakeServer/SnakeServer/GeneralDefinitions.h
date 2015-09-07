#pragma once

#include "boost\asio.hpp"
#include <string>
#include <vector>

using namespace boost::asio;

const int MAX_PLAYER_COUNT = 8;
const int FIXED_UPDATE_DELTA_TIME = 50;
const int SERVER_PORT = 10000;
const int GAME_FIELD_WIDTH = 79;
const int GAME_FIELD_HEIGHT = 24;
const char END_OF_PACKET = '\0';
const char SNAKE_HEAD = '@';
struct SnakeBlock
{
	int id;
	COORD pos;

	SnakeBlock(int n, int x, int y)
	{
		id = n;
		pos.X = x;
		pos.Y = y;
	};
};

typedef boost::shared_ptr<ip::tcp::socket> socket_ptr;
typedef std::vector<std::string> bytes;

enum EPacketType
{
	start_info = 1,
	client_info = 2,
	bonus_info = 3,
	client_eat_bonus = 4,
	add_client = 5
};

enum EDirection
{
	up = 1,
	right,
	down,
	left
};