#include "stdafx.h"
#include "Client.h"


Client::Client(int id, socket_ptr peer)
{
	this->id = id;
	this->peer = peer;
}

void Client::ReadData()
{
}

void Client::ProcessPackets()
{
}

int Client::Id()
{
	return id;
}