#pragma once

#include "ServerENet.h"
#include "AgarioData.h"
#include <Windows.h>

using namespace ENet;

class Server
{
public:
	Server();
	~Server();

	bool Init();
	void Update();

private:
	CServerENet * pServer;
	std::vector<Ball> balls;

	std::vector<CPeerENet*> peers;

	void SendWorld(CPeerENet* peer);
	void UpdateClients();
	
	size_t updateRateT;
	size_t elapsedUpdateT;
};