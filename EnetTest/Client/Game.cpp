#include "Game.h"
#include "stdafx.h"

#define PORT 1234
#define CHANNELS 2

Game::Game() {
	pClient = new CClienteENet();
	pClient->Init();
	pPeer = pClient->Connect("127.0.0.1", PORT, CHANNELS);
	pClient->Service(incomingPackets, 0);
	pClient->SendData(pPeer, "pepe", 4, 0, false);
}

Game::~Game() {
	pClient->Disconnect(pPeer);
	delete pClient;
}

void Game::Update() {
	std::vector<CPacketENet*> incomingPackets;
	pClient->Service(incomingPackets, 0);
}

void Game::Render() {

}