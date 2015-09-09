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
	io_service service;
	socket_ptr peer;
	std::vector<Player> remotePlayers;
	LocalPlayer* localPlayer;
	boost::thread serviceThread;
	double currentTime;
	int timeForMove;
	EGameState gameState;
	Field field;
	bool canChangeDirection;
	boost::mutex lpMutex;
	std::vector<COORD> bonusList;
	std::string bytesForSend;
	
	void FixedUpdate();
	Player* GetPlayer(int);
	void MovePlayer();
	bool CheckPosForBonus(COORD&);
	std::string EatBonusSerialize(COORD&);
	void SendData();
	void ReceiveData();
	void Connect();
public:
	Engine();
	void Init();
	EGameState GameState() const;
	void SetLocalDirection(EDirection);
};