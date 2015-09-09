#pragma once

#include "boost\asio.hpp"
#include <string>
#include <vector>
#include <sstream>

using namespace boost::asio;

const int MAX_PLAYER_COUNT = 8;
const int FIXED_UPDATE_DELTA_TIME = 50;
const int SERVER_PORT = 10000;
const std::string SERVER_IP = "10.44.2.96";
const int GAME_FIELD_WIDTH = 79;
const int GAME_FIELD_HEIGHT = 24;
const char END_OF_PACKET = '|';

typedef boost::shared_ptr<ip::tcp::socket> socket_ptr;
typedef char* bytes;

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

template<typename T>
std::string ToString(T value)
{
	std::ostringstream ss;
	ss << value;
	return ss.str();
};

inline std::vector<int> SplitStringToInt(std::string source, char splitSymbol)
{
	std::vector<int> res;
	while (source.length() > 0)
	{
		size_t pos = source.find(',');
		res.push_back(std::atoi(source.substr(0, pos).c_str()));
		source.erase(0, pos + 1);
	}
	return res;
}

template<typename T>
std::string VectorToString(std::vector<T> source)
{
	std::string res;
	for (auto it = source.begin(); it != source.end(); it++)
		res += *it;
	return res;
}