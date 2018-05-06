#include "Game.h"
#include "stdafx.h"
#include "Buffer.h"
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
			// Fill buffer with received data
            CBuffer* buffer = new CBuffer();
            buffer->Write(packet->GetData(), packet->GetDataLength());
            buffer->GotoStart();

            // Deserialize according to msgType
            MsgType msgType;
            buffer->Read(&msgType, sizeof(MsgType));
            if (msgType == MsgType::WORLD)
                ParseWorldMsg(buffer);
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

void Game::ParseWorldMsg(CBuffer* buffer) {
    size_t numBalls;
    buffer->Read(&numBalls, sizeof(size_t));
    for (size_t i = 0; i<numBalls; ++i) {
        Ball ball(0,0,0);
        buffer->Read(&ball, sizeof(Ball));
        balls.push_back(ball);
    }
}
