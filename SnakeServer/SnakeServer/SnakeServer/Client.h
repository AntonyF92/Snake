#include "GeneralDefinitions.h"
#include "boost\asio.hpp"
#include "MyContainers.h"
#include <string>

class Client
{
private:
	int id;
	socket_ptr peer;
	std::vector<COORD> body;
	EDirection currentDirection;
public:
	Client(int, socket_ptr);
	void ReadData();
	std::vector<COORD>& Body();
	int Id();
};
