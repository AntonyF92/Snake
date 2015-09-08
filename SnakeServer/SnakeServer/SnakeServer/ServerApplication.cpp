#include "stdafx.h"
#include "ServerApplication.h"
#include "GeneralDefinitions.h"


void ServerApplication::Init()
{
	addPlayerId = 0;
	ep = ip::tcp::endpoint(ip::tcp::v4(), SERVER_PORT);
	ip::tcp::acceptor acceptor(service, ep);
	serviceThread = boost::thread(boost::bind(&ServerApplication::Work, this));
	while (true)
	{
		socket_ptr peer(new ip::tcp::socket(service));
		acceptor.accept(*peer);
		Client cl(addPlayerId++, peer);
		players.Add(cl);
	}
}

void ServerApplication::Work()
{
}