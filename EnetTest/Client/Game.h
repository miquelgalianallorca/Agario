#pragma once

#include "ClientENet.h"
#include "AgarioData.h"
#include <Windows.h>

using namespace ENet;

class Game {
public:
	Game();
	~Game();

	void Update();
	void Render();

private:
	CClienteENet* pClient;
	CPeerENet*    pPeer;

	std::vector<Ball> balls;
	unsigned int ballTexture;
};