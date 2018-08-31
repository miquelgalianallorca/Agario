#include "Game.h"
#include "stdafx.h"
#include "Buffer.h"
#include "swalib/sys.h"
#include "swalib/core.h"
#include "swalib/font.h"
#include <iostream>
#include "AgarioSerialization.h"

#define PORT 1234
#define CHANNELS 2

using std::cout;
using std::endl;

Game::Game() :
	ID(0)
{
	pClient = new CClienteENet();
	pClient->Init();
	pPeer = pClient->Connect("127.0.0.1", PORT, CHANNELS);
	// pClient->SendData(pPeer, "pepe", 4, 0, false);
	
	ballTexture   = CORE_LoadPNG("../data/greyball.png", false);
	playerTexture = CORE_LoadPNG("../data/greenball.png", false);
	enemyTexture  = CORE_LoadPNG("../data/yellowball.png", false);

	FONT_Init();
}

Game::~Game() {
	pClient->Disconnect(pPeer);
	
	CORE_UnloadPNG(ballTexture);
	CORE_UnloadPNG(playerTexture);
	CORE_UnloadPNG(enemyTexture);

	FONT_End();
}

void Game::Update() {
	// Receive messages =============================================
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
			if (msgType == MsgType::WORLD) {
				DeserializeWorld(buffer);
			}
			else if (msgType == MsgType::UPDATE) {
				balls.clear();
				DeserializeWorld(buffer);
				//AgarioSerialization::DeserializeWorld(*buffer, balls);
			}
			else if (msgType == MsgType::ID) {
				AgarioSerialization::DeserializeID(*buffer, ID);
			}

			delete buffer;
		}
	}
	// =============================================================

	// Send messages ===============================================
	CBuffer* buffer = new CBuffer();
	ivec2 mousePos = SYS_MousePos();
	AgarioSerialization::SerializeMousePos(*buffer, ID, (float)mousePos.x, (float)mousePos.y);
	pClient->SendData(pPeer, buffer->GetBytes(), buffer->GetSize(), 0, false);
	delete buffer;
	// =============================================================

	// Check death =================================================
	bool alive = false;
	for (auto ball : balls) {
		if (ball.playerID == ID)
			alive = true;
	}
	if (!alive && balls.size() > 0) exit(0);
	// =============================================================

	Sleep(10);
}

void Game::Render() {
	// Draw balls ==================================================
	std::vector<Ball*> playerBalls;
	unsigned int texture = ballTexture;
	// Draw food balls
	for (auto& ball : balls) {
		if (ball.type == BallType::PLAYER) {
			playerBalls.push_back(&ball);
		}
		else {
			CORE_RenderCenteredRotatedSprite(vmake(ball.posX, ball.posY),
				vmake(ball.size, ball.size), 0.f, ballTexture);
		}
	}
	// Draw player's balls
	for (auto ball : playerBalls) {
		unsigned int texture = enemyTexture;
		if (ball->playerID == ID) texture = playerTexture;
		
		CORE_RenderCenteredRotatedSprite(vmake(ball->posX, ball->posY),
			vmake(ball->size, ball->size), 0.f, texture);
	}
	// =============================================================

	// FONT_DrawString(vmake(0.f, 0.f), "AAAAAAAAA");
}

void Game::DeserializeWorld(CBuffer* buffer) {
    size_t numBalls;
    buffer->Read(&numBalls, sizeof(size_t));
    for (size_t i = 0; i<numBalls; ++i) {
        Ball ball(0, 0, 0, 0.f, BallType::FOOD);
        buffer->Read(&ball, sizeof(Ball));
        balls.push_back(ball);
    }
}
