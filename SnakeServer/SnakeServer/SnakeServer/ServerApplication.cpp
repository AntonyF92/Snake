#include "stdafx.h"
#include "ServerApplication.h"
#include "GeneralDefinitions.h"


static void Work()
{
	
}

void ServerApplication::Init()
{
	ep = ip::tcp::endpoint(ip::tcp::v4(), SERVER_PORT);
	ip::tcp::acceptor acceptor(service, ep);
	serviceThread = boost::thread(&Work);
	while (true)
	{
		socket_ptr peer(new ip::tcp::socket(service));
		acceptor.accept(*peer);
		Client cl(peer);
		players.Add(cl);
	}
}
