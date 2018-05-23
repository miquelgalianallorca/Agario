#pragma once

#include "ClientENet.h"
#include "AgarioData.h"
#include <Windows.h>

using namespace ENet;
class CBuffer;

class Game {
public:
	Game();
	~Game();

	void Update();
	void Render();

private:
    void DeserializeWorld(CBuffer* buffer);

	CClienteENet* pClient;
	CPeerENet*    pPeer;

	size_t ID;
	std::vector<Ball> balls;

	unsigned int ballTexture;   // Food balls
	unsigned int playerTexture; // Client ball
	unsigned int enemyTexture;  // Other clients
};