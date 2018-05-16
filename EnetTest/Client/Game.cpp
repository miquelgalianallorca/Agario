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

Game::Game() :
	ID(0)
{
	pClient = new CClienteENet();
	pClient->Init();
	pPeer = pClient->Connect("127.0.0.1", PORT, CHANNELS);
	pClient->SendData(pPeer, "pepe", 4, 0, false);
	
	ballTexture   = CORE_LoadPNG("../data/greyball.png", false);
	playerTexture = CORE_LoadPNG("../data/greenball.png", false);
	enemyTexture  = CORE_LoadPNG("../data/yellowball.png", false);
}

Game::~Game() {
	pClient->Disconnect(pPeer);
	delete pClient;

	CORE_UnloadPNG(ballTexture);
	CORE_UnloadPNG(playerTexture);
	CORE_UnloadPNG(enemyTexture);
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
			}
			else if (msgType == MsgType::ID) {
				buffer->Read(&ID, sizeof(size_t));
			}

			delete buffer;
		}
	}
	// =============================================================

	// Send messages ===============================================
	CBuffer* buffer = SerializeMousePos();
	pClient->SendData(pPeer, buffer->GetBytes(), buffer->GetSize(), 0, false);
	delete buffer;
	// =============================================================

	//Sleep(10);
}

void Game::Render() {
	for (auto ball : balls) {
		// Players in another color
		unsigned int texture = ballTexture;
		if (ball.type == BallType::PLAYER) {
			texture = enemyTexture;
			if (ball.playerID == ID) texture = playerTexture;
		}
		
		// Draw
		CORE_RenderCenteredRotatedSprite(vmake(ball.posX, ball.posY),
			vmake(ball.radius, ball.radius), 0.f, texture);
	}
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

CBuffer* Game::SerializeMousePos() {
	CBuffer* buffer = new CBuffer();

	MsgType msgType = MsgType::MOVE;
	buffer->Write(&msgType, sizeof(MsgType));

	buffer->Write(&ID, sizeof(ID));

	ivec2 mousePos = SYS_MousePos();
	float posX = (float)mousePos.x;
	float posY = (float)mousePos.y;
	buffer->Write(&posX, sizeof(float));
	buffer->Write(&posY, sizeof(float));

	buffer->GotoStart();
	return buffer;
}