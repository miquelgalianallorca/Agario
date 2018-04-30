#pragma once

#include "ServerENet.h"
#include <Windows.h>

using namespace ENet;

class Server {
public:
	struct Ball {
		Ball(float _x, float _y, float _rad) : posX(_x), posY(_y), radius(_rad) {}
		float posX;
		float posY;
		float radius;
	};

	Server();
	~Server();

	bool Init();
	void Update();

private:
	CServerENet * pServer;
	std::vector<Ball*> balls;

	std::vector<CPeerENet*> peers;
};