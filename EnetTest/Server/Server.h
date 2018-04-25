#pragma once

#include "ServerENet.h"
#include <Windows.h>

using namespace ENet;

class Server {
public:
	class Ball {

	};

	Server();
	~Server();

	bool Init();
	void Update();

private:
	CServerENet * pServer;
	std::vector<Ball*> balls;
};