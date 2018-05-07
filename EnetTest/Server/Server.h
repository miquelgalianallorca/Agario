#pragma once

#include "ServerENet.h"
#include "AgarioData.h"
#include <Windows.h>

using namespace ENet;

class CBuffer;

class Server {
public:
	struct Client {
		Client(CPeerENet* _peer, Ball* _ball) : peer(_peer), ball(_ball) {}
		CPeerENet* peer;
		Ball* ball;
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

	CBuffer* SerializeWorld(MsgType msgType);
	void SendWorld(CPeerENet* peer);
	void UpdateClients();
	void DeserializeMousePos(CPeerENet* peer, CBuffer* buffer);
	
	size_t updateRateT;
	size_t elapsedUpdateT;
};