#include "GeneralDefinitions.h"
#include <vector>
#include "Player.h"
#include "LocalPlayer.h"
#include "boost\thread.hpp"
#include "boost\chrono.hpp"
#include "boost\timer.hpp"


class Engine
{
private:
	socket_ptr peer;
	std::vector<Player> remotePlayers;
	LocalPlayer localPlayer;
	boost::thread serviceThread;
	double currentTime;
	int timeForMove;
	void FixedUpdate();
	//Player& GetPlayer(int);
	void MovePlayer(LocalPlayer&);
public:
	void Init();
};