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
		float posX = (float)(std::rand() % 1000);
		float posY = (float)(std::rand() % 1000);
		balls.push_back(new Ball(posX, posY, BALLSIZE, BallType::FOOD));
	}
}

Server::~Server() {
	// Clear game
	for (auto ball : balls)
		delete ball;
	balls.clear();
	
	// Clear network
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
			AddClient(packet->GetPeer());
		}
		else if (packet->GetType() == EPacketType::DATA) {
			CBuffer* buffer = new CBuffer();
			buffer->Write(packet->GetData(), packet->GetDataLength());
			buffer->GotoStart();

			// Deserialize data according to msg type
			MsgType msgType;
			buffer->Read(&msgType, sizeof(MsgType));
			if (msgType == MsgType::MOVE) {
				DeserializeMousePos(packet->GetPeer(), buffer);
			}

			delete buffer;
		}
		else if (packet->GetType() == EPacketType::DISCONNECT) {
			RemoveClient(packet->GetPeer());
		}
	}

	// Write to all peers
	UpdateClients();
	Sleep(DELTA_TIME);
}

CBuffer* Server::SerializeWorld(MsgType msgType) {
	CBuffer* buffer = new CBuffer();

	// Message type
	buffer->Write(&msgType, sizeof(MsgType));
	
	// Balls
	size_t numBalls = balls.size();
	buffer->Write(&numBalls, sizeof(size_t));
	for (auto ball : balls)
		buffer->Write(ball, sizeof(Ball));
	
	buffer->GotoStart();
	return buffer;
}

void Server::SendWorld(CPeerENet* peer) {
	// Snapshot of all entities (reliable)
	CBuffer* buffer = SerializeWorld(MsgType::WORLD);
    pServer->SendData(peer, buffer->GetBytes(), buffer->GetSize(), 1, true);
}

void Server::UpdateClients() {
	if (elapsedUpdateT >= updateRateT) {
		elapsedUpdateT = 0;

		// Snapshot of all entities (non reliable)
		CBuffer* buffer = SerializeWorld(MsgType::UPDATE);
		pServer->SendAll(buffer->GetBytes(), buffer->GetSize(), 0, false);
	}
	else elapsedUpdateT += DELTA_TIME;
}

void Server::AddClient(CPeerENet* peer) {
	Ball* ball = new Ball(STARTPOSX, STARTPOSY, BALLSIZE, BallType::PLAYER);
	// Each packet has owner Peer (UDP)
	clients.push_back(Client(peer, ball));
	balls.push_back(ball);
	// Send snapshot of the world to the client
	SendWorld(peer);
}

void Server::RemoveClient(CPeerENet* peer) {
	clients.erase(std::remove_if(clients.begin(), clients.end(),
		[peer](Client &c) { return c.peer == peer; }), clients.end());
	pServer->Disconnect(peer);
}

void Server::DeserializeMousePos(CPeerENet* peer, CBuffer* buffer) {
	float destX = 0.f;
	float destY = 0.f;
	buffer->Read(&destX, sizeof(float));
	buffer->Read(&destY, sizeof(float));

	// Find that player
	auto it = std::find_if(clients.begin(), clients.end(),
		[peer](Client &c) { return c.peer = peer; });
	
	// Move him
	float posX = it->ball->posX;
	float posY = it->ball->posY;
	float velX = destX - posX; //Falta normalizar y multiplicar por velocidad
	float velY = destY - posY;
	it->ball->posX += velX;
	it->ball->posY += velY;
}