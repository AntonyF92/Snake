#include "GeneralDefinitions.h"
#include "boost\asio.hpp"
#include <string>

class Client
{
private:
	int id;
	socket_ptr peer;
	std::vector<COORD> body;
	EDirection currentDirection;
	bool directionChanged;
public:
	bool disconnected;
	Client(int, socket_ptr);
	std::string ReadData();
	std::vector<COORD>& Body();
	int Id();
	bool DirectionChanged();
	EDirection CurrentDirection();
	std::string Serialize();
	std::string SerializeFull(bool for_start = false);
	std::string SerializeDelete();
	socket_ptr Peer();
};
