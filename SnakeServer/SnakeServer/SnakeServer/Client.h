#include "GeneralDefinitions.h"
#include "boost\asio.hpp"
#include "MyContainers.h"

class Client
{
private:
	int id;
	socket_ptr peer;
	Dictionary<EPacketType, bytes> packets;
	void ProcessPackets();
public:
	bytes bytesForSend;
	Client(int, socket_ptr);
	void ReadData();
	int Id();
};
