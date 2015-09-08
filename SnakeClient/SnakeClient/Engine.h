#include "GeneralDefinitions.h"
#include <vector>
#include "Player.h"
#include "LocalPlayer.h"
#include "boost\thread.hpp"
#include "boost\chrono.hpp"
#include "boost\timer.hpp"
#include "Field.h"


class Engine
{
private:
	socket_ptr peer;
	std::vector<Player> remotePlayers;
	LocalPlayer localPlayer;
	boost::thread serviceThread;
	double currentTime;
	int timeForMove;
	EGameState gameState;
	Field field;
	bool canChangeDirection;
	boost::mutex lpMutex;
	std::vector<COORD> bonusList;
	
	void FixedUpdate();
	//Player& GetPlayer(int);
	void MovePlayer(LocalPlayer&);
	bool CheckPosForBonus(COORD&);
public:
	void Init();
	EGameState GameState() const;
	void SetLocalDirection(EDirection);
};