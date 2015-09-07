#include "GeneralDefinitions.h";

class Player
{
protected:
	int id;
	std::vector<SnakeBlock> body;
	bool local;
public:
	Player(bool);
	std::vector<SnakeBlock> Body() const;
	bool IsLocal() const;
};