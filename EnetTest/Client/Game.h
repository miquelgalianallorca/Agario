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
    void ParseWorldMsg(CBuffer* buffer);

	CClienteENet* pClient;
	CPeerENet*    pPeer;

	std::vector<Ball> balls;
	unsigned int ballTexture;
};