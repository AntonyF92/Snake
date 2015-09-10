#pragma once
#include "GeneralDefinitions.h";
#include "MemoryBuffer.h"

class Player
{
protected:
	int id;
	std::vector<COORD> body;
	EDirection currentDirection;
	bool local;
public:
	Player(int, std::vector<COORD>);
	const std::vector<COORD>& Body();
	bool drawRemoveTail;
	void RemoveTail();
	void Move();
	EDirection CurrentDirection() const;
	bool SetDirection(EDirection);
	std::vector<COORD> OldBody();
	void AddBlock(COORD);
	NetPacket* Serialize();
	int Id();
	bool IsLocal() const;
};

