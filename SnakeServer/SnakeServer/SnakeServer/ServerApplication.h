#include "boost/asio.hpp"
#include "GeneralDefinitions.h"
#include "Client.h"
#include "boost/thread.hpp"

using namespace boost::asio;

class ServerApplication
{
private:
	int addPlayerId;
	ConcurrentVector<Client> players;
	io_service service;
	ip::tcp::endpoint ep;
	boost::thread serviceThread;
public:
	void Init();
	void Work();
};