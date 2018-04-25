#include "Game.h"
#include "stdafx.h"

using namespace ENet;

Game::Game() {
	pClient = new CClienteENet();
	pClient->Init();
	pPeer = pClient->Connect("127.0.0.1", 1234, 2);
	pClient->Service(incomingPackets, 0);
	pClient->SendData(pPeer, "pepe", 4, 0, false);
}

Game::~Game() {
	pClient->Disconnect(pPeer);
	delete pClient;
}

void Game::Update() {
	std::vector<CPacketENet*>  incomingPackets;
	pClient->Service(incomingPackets, 0);
}

void Game::Render() {

}