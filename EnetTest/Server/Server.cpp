#include "Server.h"
#include "stdafx.h"
#include "Buffer.h"

#include <algorithm>
#include <iostream>
#include <ctime>

#define PORT 1234
#define CLIENTS 5

#define STARTING_BALLS 20
#define DELTA_TIME 100

using std::cout;
using std::endl;

Server::Server() : updateRateT(1000), elapsedUpdateT(0) {
	pServer = new CServerENet();

	// Load starting balls
	std::srand((unsigned int)std::time(nullptr));
	for (size_t i = 0; i < STARTING_BALLS; ++i)	{
		float posX = (float)(std::rand() % 100);
		float posY = (float)(std::rand() % 100);
		balls.push_back(Ball(posX, posY, 10.f));
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
	
	for (auto packet : incomingPackets)	{
		if (packet->GetType() == EPacketType::CONNECT) {
			// Each packet has owner Peer (UDP)
			peers.push_back(packet->GetPeer());
			// Send snapshot of the world to the client
			SendWorld(packet->GetPeer());
		}
		else if (packet->GetType() == EPacketType::DATA) {
			size_t dataLength = packet->GetDataLength();
			cout << dataLength << endl;

			char *buffer = new char[dataLength + 1];
			memcpy(buffer, packet->GetData(), dataLength);
			buffer[dataLength] = '\0';
			cout << buffer << endl;
		}
		else if (packet->GetType() == EPacketType::DISCONNECT) {
			// Remove client
			peers.erase(std::remove(peers.begin(), peers.end(), packet->GetPeer()), peers.end());
			pServer->Disconnect(packet->GetPeer());
			break;
		}
	}

	// Write to all peers
	UpdateClients();
}

void Server::SendWorld(CPeerENet* peer) {
    CBuffer* buffer = new CBuffer();
    // Type
    MsgType msgType = MsgType::WORLD;
    buffer->Write(&msgType, sizeof(MsgType));	
    // Balls
    size_t numBalls = balls.size();
    buffer->Write(&numBalls, sizeof(size_t));
    for (auto ball : balls) {
        buffer->Write(&ball, sizeof(Ball));
    }
    // Send
    buffer->GotoStart();
    pServer->SendData(peer, buffer->GetBytes(),
        buffer->GetSize(), 1, true);
}

void Server::UpdateClients() {
	if (elapsedUpdateT >= updateRateT) {
		elapsedUpdateT = 0;
		//pServer->SendAll("AA", 2, 0, false);
	}
	else elapsedUpdateT += DELTA_TIME;
	Sleep(DELTA_TIME);
}