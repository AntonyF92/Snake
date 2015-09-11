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
	unsigned char* memBuffer;
public:
	bool justAdded;
	Client(int, socket_ptr);
	void ReadData();
	std::vector<COORD>& Body();
	int Id();
	bool DirectionChanged();
	EDirection CurrentDirection();
	NetPacket Serialize();
	NetPacket* SerializeFull();
	socket_ptr Peer();
};
