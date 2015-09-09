#include "Player.h"
#include "GeneralDefinitions.h"
#include  "Constants.h"

class LocalPlayer : public Player
{
private:
public:
	LocalPlayer(int, std::vector<COORD>);
	COORD HeadPosition() const;
	bool CheckCollision(COORD&);
};