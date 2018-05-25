#pragma once

#include "ServerENet.h"
#include "AgarioData.h"
#include <Windows.h>

using namespace ENet;

class CBuffer;

class Server {
public:
	struct Client {
		Client(CPeerENet* _peer, Ball* _ball, size_t _ID);
		CPeerENet* peer;
		Ball* ball;
		size_t ID;
	};
	
	Server();
	~Server();

	bool Init();
	void Update();

private:
	CServerENet* pServer;
	std::vector<Ball*> balls;
	std::vector<Client> clients;

	void AddClient(CPeerENet* peer);
	void RemoveClient(CPeerENet* peer);

	void SendWorld(CPeerENet* peer);
	void SendID(CPeerENet* peer, size_t ID);
	void UpdateBalls();
	void UpdateClients();
	void MovePlayerToMousePos(CBuffer* buffer);
	
	size_t updateRateT;
	size_t elapsedUpdateT;
};