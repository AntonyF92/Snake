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
	void SendToAll(TotalPacket*, size_t);
	void Work();
	void CopyPacket(NetPacket& in, NetPacket& out);
	void CheckDisconnects();
public:
	void Init();
};