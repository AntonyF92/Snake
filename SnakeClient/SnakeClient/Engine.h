#include "GeneralDefinitions.h"
#include <vector>
#include "Player.h"
#include "LocalPlayer.h"
#include "boost\thread.hpp"
#include "boost\chrono.hpp"
#include "boost\timer.hpp"
#include "Field.h"
#include "MemoryBuffer.h"


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
	NetPacket* packetForSend;
	
	void FixedUpdate();
	Player* GetPlayer(int);
	void MovePlayer();
	bool CheckPosForBonus(COORD&);
	NetPacket* EatBonusSerialize(COORD&);
	void SendData();
	void ReceiveData();
	void Connect();
public:
	Engine();
	void Init();
	EGameState GameState() const;
	void SetLocalDirection(EDirection);
	void PrintString(const char* str);
};