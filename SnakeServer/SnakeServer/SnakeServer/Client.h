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
public:
	bool disconnected;
	bool finished;
	bool directionChanged;
	int scores;
	Client(int, socket_ptr);
	void ReadData();
	std::vector<COORD>& Body();
	int Id();
	EDirection CurrentDirection();
	std::vector<int> Serialize();
	std::vector<int> SerializeFull(bool for_start = false);
	std::vector<int> SerializeDelete();
	socket_ptr Peer();
	void SetDirection(EDirection);
	void Move();
	COORD HeadPosition() const;
	void AddBlock(COORD);
	bool CheckCollision(COORD&);
};
