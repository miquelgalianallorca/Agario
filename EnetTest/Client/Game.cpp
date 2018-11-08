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
	ID(0),
	isDeadReckoningOn(true)
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

	// Dead reckoning ==============================================
	if (isDeadReckoningOn) Interpolate();
	// =============================================================
	
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
				// Clear
				ballsFromServer.clear();
				// Deserialize
				DeserializeWorld(buffer);
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
	for (auto ball : ballsFromServer) {
		if (ball->playerID == ID)
			alive = true;
	}
	if (!alive && ballsFromServer.size() > 0) exit(0);
	// =============================================================

	Sleep(10);
}

void Game::Render() {
	// Draw balls ==================================================
	std::vector<Ball*> playerBalls;
	unsigned int texture = ballTexture;
	// Draw food balls
	if (!isDeadReckoningOn) {
		for (auto& ball : ballsFromServer) {
			if (ball->type == BallType::PLAYER) {
				playerBalls.push_back(ball);
			}
			else {
				CORE_RenderCenteredRotatedSprite(vmake(ball->posX, ball->posY),
					vmake(ball->size, ball->size), 0.f, ballTexture);
			}
		}
		// Draw player's balls
		for (auto ball : playerBalls) {
			unsigned int texture = enemyTexture;
			if (ball->playerID == ID) texture = playerTexture;

			CORE_RenderCenteredRotatedSprite(vmake(ball->posX, ball->posY),
				vmake(ball->size, ball->size), 0.f, texture);
		}
	}

	else {
		for (auto ball : ballsFromServer) {
			if (ball->type == BallType::FOOD) {
				CORE_RenderCenteredRotatedSprite(vmake(ball->posX, ball->posY),
					vmake(ball->size, ball->size), 0.f, ballTexture);
			}
		}
		for (auto ball : ballsLocalPlayers) {
			unsigned int texture = enemyTexture;
			if (ball->playerID == ID) texture = playerTexture;
			CORE_RenderCenteredRotatedSprite(vmake(ball->posX, ball->posY),
				vmake(ball->size, ball->size), 0.f, texture);
		}
	}
	// =============================================================
}

void Game::DeserializeWorld(CBuffer* buffer) {
    size_t numBalls;
    buffer->Read(&numBalls, sizeof(size_t));
    for (size_t i = 0; i<numBalls; ++i) {
        Ball* ball = new Ball(0, 0, 0, 0.f, BallType::FOOD);
        buffer->Read(ball, sizeof(Ball));

		// Dead reckoning
		if (isDeadReckoningOn) {
			// Player ball
			if (ball->playerID > 0) {
				// Add to localPlayers if not beforehand
				auto it = map.find(ball->playerID);
				if (it == map.end()) {
					Ball* ballCopy = new Ball(*ball);
					map.insert(std::pair<size_t, Ball*>(ballCopy->playerID, ballCopy));
					ballsLocalPlayers.push_back(ballCopy);
				}
				ballsFromServer.push_back(ball);
			}
			// Food ball
			else {
				ballsFromServer.push_back(ball);
			}
		}

		// No dead reckoning
		else {
			ballsFromServer.push_back(ball);
		}
    }
}

void Game::Interpolate() {
	// Advance local balls to server positions
	for (auto ballFromServer : ballsFromServer) {
		// Player found
		if (map.find(ballFromServer->playerID) != map.end()) {
			Ball* ballLocal = map.at(ballFromServer->playerID);
			// Interpolate ball towards ballInterp
			if (ballLocal) {
				// Update size
				ballLocal->size = ballFromServer->size;
				// Update position
				vec2 pos0 = vmake(ballFromServer->posX, ballFromServer->posY);
				vec2 pos1 = vmake(ballLocal->posX, ballLocal->posY);
				vec2 dir = vsub(pos0, pos1);
				float len = vlen2(dir);
				// Epsilon to stop interpolation in proximity
				if (len > 3.f) {
					vec2 norm = vmake(dir.x / len, dir.y / len);
					ballLocal->posX = pos0.x + norm.x * ballLocal->speed;
					ballLocal->posY = pos0.y + norm.y * ballLocal->speed;
				}
			}
		}
	}
}