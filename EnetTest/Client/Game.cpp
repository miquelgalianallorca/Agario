#include "Game.h"
#include "stdafx.h"
#include "swalib\sys.h"
#include "swalib\core.h"
#include <iostream>

#define PORT 1234
#define CHANNELS 2

using std::cout;
using std::endl;

Game::Game() {
	pClient = new CClienteENet();
	pClient->Init();
	pPeer = pClient->Connect("127.0.0.1", PORT, CHANNELS);
	pClient->SendData(pPeer, "pepe", 4, 0, false);

	ballTexture = CORE_LoadPNG("../data/ball.png", false);
	//balls.push_back(Ball(100.f, 100.f, 100.f)); //test
}

Game::~Game() {
	pClient->Disconnect(pPeer);
	delete pClient;

	CORE_UnloadPNG(ballTexture);
}

void Game::Update() {
	std::vector<CPacketENet*> incomingPackets;
	pClient->Service(incomingPackets, 0);

	for (auto packet : incomingPackets)	{
		if (packet->GetType() == EPacketType::DATA)	{
			byte* data = packet->GetData();
			cout << data << endl;
			// no reinterpret casts!
			// hace falta una clase base msg con un enum msgType (4 bytes)
			// luego hay clases hijas que se serializan para hacer un chorizo de bytes
			// del resto de la informacion.
			// (todas tienen en comun que los 4 primeros son el tipo
			// segun el tipo deserializas el resto de la informacion.
			/*if (auto msg = reinterpret_cast<MsgWorld*>(data))
			{
				int i = 0;
			}*/
		}
	}

	Sleep(100);
}

void Game::Render() {
	for (auto ball : balls) {
		CORE_RenderCenteredRotatedSprite(vmake(ball.posX, ball.posY),
			vmake(ball.radius, ball.radius), 0.f, ballTexture);
	}
}