#include "boost/asio.hpp"
#include "GeneralDefinitions.h"
#include "Client.h"
#include "boost/thread.hpp"
#include "boost\timer.hpp"
#include <ctime>

using namespace boost::asio;

class ServerApplication
{
private:
	int addPlayerId;
	std::vector<Client> players;
	io_service service;
	ip::tcp::endpoint ep;
	boost::thread serviceThread;
	boost::mutex plMutex;
	double currentTime, timeForMove, timeForGenerateBonus;
	std::vector<COORD> bonusList;
	bool bonusLocked;
	int maxScores;
	bool scoreChanged;

	bool CreateBody(Client&);
	void SendToAll(std::vector<int>);
	void Work();
	void CopyPacket(NetPacket& in, NetPacket& out);
	std::vector<int> CheckDisconnects();
	std::vector<int> MovePlayer(Client&);
	bool CheckPosForBonus(COORD&);
	//std::string EatBonusSerialize(COORD&);
	bool CheckHeadPos(COORD&);
	void GenerateBonuses();
	std::vector<int> SerializeBonuses();
	void RemoveBonus(COORD&);
	void SendScores();
	std::vector<int> SerializeScoresForPlayer(Client&);
public:
	std::string packetForSend;
	void Init();
};