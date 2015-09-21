#include "GeneralDefinitions.h"
#include <vector>
#include "Player.h"
#include "LocalPlayer.h"
#include "boost\thread.hpp"
#include "boost\chrono.hpp"
#include "boost\timer.hpp"
#include "Field.h"
#include "MemoryBuffer.h"
#include "boost\regex.hpp"


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
	std::vector<int> packetForSend;
	
	void FixedUpdate();
	Player* GetPlayer(int);
	void MovePlayer();
	bool CheckPosForBonus(COORD&);
	std::string EatBonusSerialize(COORD&);
	void SendData();
	void ReceiveData();
	void Connect();
	std::vector<COORD> CalcBody(COORD, COORD);
public:
	Engine();
	void Init();
	EGameState GameState() const;
	void SetLocalDirection(EDirection);
	void PrintString(const char* str);
	void Close();
};