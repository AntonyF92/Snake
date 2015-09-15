#pragma once
#include "GeneralDefinitions.h";
#include "MemoryBuffer.h"

class Player
{
protected:
	int id;
	std::vector<COORD> body;
	std::vector<COORD> oldBody;
	EDirection currentDirection;
	bool local;
public:
	Player(int, std::vector<COORD>);
	std::vector<COORD>& Body();
	bool drawRemoveTail;
	void RemoveTail();
	void Move();
	EDirection CurrentDirection() const;
	bool SetDirection(EDirection);
	std::vector<COORD> OldBody();
	void AddBlock(COORD);
	std::string Serialize();
	int Id();
	bool IsLocal() const;
	void SaveBody();
};

