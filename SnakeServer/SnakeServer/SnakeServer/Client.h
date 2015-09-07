#include "GeneralDefinitions.h"
#include "boost\asio.hpp"
#include "MyContainers.h"

class Client
{
private:
	socket_ptr peer;
	Dictionary<EPacketType, bytes> packets;
	void ProcessPackets();
public:
	bytes bytesForSend;
	Client(socket_ptr);
	void ReadData();
};
