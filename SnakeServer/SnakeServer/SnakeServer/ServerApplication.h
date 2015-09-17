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

	void CreateBody(Client&);
	void SendToAll(std::string packet);
	void Work();
	void CopyPacket(NetPacket& in, NetPacket& out);
	std::string CheckDisconnects();
	std::string MovePlayer(Client&);
	bool CheckPosForBonus(COORD&);
	//std::string EatBonusSerialize(COORD&);
	bool CheckHeadPos(COORD&);
	void GenerateBonuses();
	std::string SerializeBonuses();
	void RemoveBonus(COORD&);
	void SendScores();
	std::string SerializeScoresForPlayer(Client&);
public:
	std::string packetForSend;
	void Init();
};