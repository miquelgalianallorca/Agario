#pragma once

#include "ClientENet.h"
#include "AgarioData.h"
#include <map>
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
	void Interpolate();

	CClienteENet* pClient;
	CPeerENet*    pPeer;

	size_t ID;
	std::vector<Ball*> ballsFromServer;
	std::vector<Ball*> ballsLocalPlayers; // Dead reckoning - Player balls destinations
	std::map<size_t, Ball*> map;

	unsigned int ballTexture;      // Food balls
	unsigned int playerTexture;    // Client ball
	unsigned int enemyTexture;     // Other clients

	bool isDeadReckoningOn;
};