#include "stdafx.h"
#include "Client.h"


Client::Client(socket_ptr peer)
{
	this->peer = peer;
}

void Client::ReadData()
{
}

void Client::ProcessPackets()
{
}