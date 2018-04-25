#include "Server.h"
#include "stdafx.h"

#define PORT 1234
#define CLIENTS 5

Server::Server() {
	pServer = new CServerENet();
}

Server::~Server() {
	delete pServer;
}

bool Server::Init() {
	return pServer->Init(PORT, CLIENTS);
}

void Server::Update() {
	std::vector<CPacketENet*>  incomingPackets;
	pServer->Service(incomingPackets, 0);
	Sleep(100);
}