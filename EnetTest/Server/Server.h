#pragma once

#include "ServerENet.h"
#include <Windows.h>

using namespace ENet;

class Server {
public:
	Server();
	~Server();

	bool Init();
	void Update();

private:
	CServerENet * pServer;
};