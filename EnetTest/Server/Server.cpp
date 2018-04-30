#include "Server.h"
#include "stdafx.h"

#include <algorithm>
#include <iostream>
#include <ctime>

#define PORT 1234
#define CLIENTS 5

#define STARTING_BALLS 20

using std::cout;
using std::endl;

Server::Server() {
	pServer = new CServerENet();

	// Load starting balls
	std::srand((unsigned int)std::time(nullptr));
	for (size_t i = 0; i < STARTING_BALLS; ++i)
	{
		float posX = (float)(std::rand() % 100);
		float posY = (float)(std::rand() % 100);
		balls.push_back(new Ball(posX, posY, 10.f));
	}
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
	
	for (auto packet : incomingPackets)
	{
		if (packet->GetType() == EPacketType::CONNECT)
		{
			// Cada paquete lleva la informacion de su Peer
			// como en UDP
			peers.push_back(packet->GetPeer());
		}
		else if (packet->GetType() == EPacketType::DATA)
		{
			size_t dataLength = packet->GetDataLength();
			cout << dataLength << endl;

			char *buffer = new char[dataLength + 1];
			memcpy(buffer, packet->GetData(), dataLength);
			buffer[dataLength] = '\0';
			cout << buffer << endl;
		}
		else if (packet->GetType() == EPacketType::DISCONNECT)
		{
			// Remove client
			peers.erase(std::remove(peers.begin(), peers.end(), packet->GetPeer()), peers.end());
			pServer->Disconnect(packet->GetPeer());
			break;
		}
	}

	// Write to all peers
	pServer->SendAll("AA", 2, 0, false);

	Sleep(100);
}