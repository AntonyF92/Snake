#pragma once

#include "boost\asio.hpp"
#include <string>
#include <vector>
#include <sstream>
#include "MemoryBuffer.h"
#include "boost\regex.hpp"

using namespace boost::asio;

const int MAX_PLAYER_COUNT = 8;
const int FIXED_UPDATE_DELTA_TIME = 50;
const int SERVER_PORT = 10000;
const std::string SERVER_IP = "10.44.2.96";
const int GAME_FIELD_WIDTH = 79;
const int GAME_FIELD_HEIGHT = 24;
const char END_OF_PACKET = '|';
const int SNAKE_MOVEMENT_DELTA_TIME = 350;
const int SNAKE_INIT_LENGTH = 6;
const int MAX_BONUS_COUNT = 5;
const int GENERATE_BONUS_DELTA_TIME = 5000;
const int BORDER_X = 0;
const int BORDER_Y = 1;

typedef boost::shared_ptr<ip::tcp::socket> socket_ptr;
typedef char* bytes;

enum EPacketType
{
	start_info = 1,
	client_info = 2,
	bonus_info = 3,
	client_eat_bonus = 4,
	add_client = 5,
	delete_player = 6,
	scores = 7
};

enum EStartError
{
	no = 1,
	max_players = 2
};

enum EDirection
{
	up = 1,
	right,
	down,
	left
};

struct NetPacket
{
	int packet_id;
	int dataSize;
	int packetSize;
	int data[1];
};


struct TotalPacket
{
	int packetCount;
	NetPacket packets[1];
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
		if (pos != std::string::npos)
			res.push_back(std::atoi(source.substr(0, pos).c_str()));
		source.erase(0, pos == std::string::npos ? std::string::npos : pos + 1);
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

inline void FormattPacket(std::string& s)
{
	boost::regex reg("[^(((\\d+\,)+\|)+)]+");
	s = boost::regex_replace(s, reg, "");
	auto pos = s.find_last_of(END_OF_PACKET);
	s = s.erase(pos == std::string::npos ? 0 : pos + 1);
}

inline bool VectorContains(std::vector<COORD>& source, COORD& value)
{
	for (auto it = source.begin();it!=source.end();it++)
	if (it->X == value.X&&it->Y == value.Y)
		return true;
	return false;
}