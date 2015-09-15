#include "boost/asio.hpp"
#include "GeneralDefinitions.h"
#include "Client.h"
#include "boost/thread.hpp"

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

	void CreateBody(Client&);
	void SendToAll(std::string packet);
	void Work();
	void CopyPacket(NetPacket& in, NetPacket& out);
	std::string CheckDisconnects();
public:
	std::string packetForSend;
	void Init();
};