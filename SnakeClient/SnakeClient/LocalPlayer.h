#include "Player.h"
#include "GeneralDefinitions.h"
#include  "Constants.h"

class LocalPlayer : public Player
{
private:
	EDirection currentDirection;
public:
	LocalPlayer();
	EDirection CurrentDirection() const;
	void SetDirection(EDirection);
	COORD HeadPosition() const;
};