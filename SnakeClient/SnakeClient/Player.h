#pragma once
#include "GeneralDefinitions.h";

class Player
{
protected:
	int id;
	std::vector<SnakeBlock> body;
	bool local;
	EDirection currentDirection;
public:
	Player(bool);
	const std::vector<SnakeBlock>& Body();
	bool IsLocal() const;
	bool drawRemoveTail;
	void RemoveTail();
	void Move(EDirection);
	EDirection CurrentDirection() const;
	bool SetDirection(EDirection);
	std::vector<SnakeBlock> OldBody();
	void AddBlock(SnakeBlock);
};

