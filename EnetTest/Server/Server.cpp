#include "Server.h"
#include "stdafx.h"
#include "Buffer.h"
#include "AgarioSerialization.h"

#include <algorithm>
#include <iostream>
#include <ctime>

#define PORT 1234
#define CLIENTS 5

#define WORLD_WIDTH 640
#define WORLD_HEIGHT 480
#define STARTING_BALLS 10
#define DELTA_TIME 10
#define INIT_PLAYER_SPEED 5

#define NEW_BALL_RATE 100
#define MAX_BALLS 20

using std::cout;
using std::endl;

Server::Server() :
	updateRateT(20),
	elapsedUpdateT(0),
	newballTimer(0)
{
	pServer = new CServerENet();

	// Load starting balls
	std::srand((unsigned int)std::time(nullptr));
	for (size_t i = 0; i < STARTING_BALLS; ++i)	{
		SpawnBall();
	}
}

Server::~Server() {
	// Clear game
	for (auto ball : balls)
		delete ball;
	balls.clear();
	
	// Clear network
	delete pServer;
}

bool Server::Init() {
	return pServer->Init(PORT, CLIENTS);
}

void Server::Update()
{
	// Receive messages
	std::vector<CPacketENet*> incomingPackets;
	pServer->Service(incomingPackets, 0);
	
	for (auto packet : incomingPackets)	{
		if (packet->GetType() == EPacketType::CONNECT) {
			AddClient(packet->GetPeer());
		}
		else if (packet->GetType() == EPacketType::DATA) {
			CBuffer* buffer = new CBuffer();
			buffer->Write(packet->GetData(), packet->GetDataLength());
			buffer->GotoStart();

			// Deserialize data according to msg type
			MsgType msgType;
			buffer->Read(&msgType, sizeof(MsgType));
			if (msgType == MsgType::MOVE) {
				MovePlayerToMousePos(buffer);
			}

			delete buffer;
		}
		else if (packet->GetType() == EPacketType::DISCONNECT) {
			RemoveClient(packet->GetPeer());
		}
	}

	// Write to all peers
	UpdateBalls();
	
	// Add balls
	++newballTimer;
	if (newballTimer > NEW_BALL_RATE)
	{
		SpawnBall();
		newballTimer = 0;
	}
	
	UpdateClients();
	Sleep(DELTA_TIME);
}

bool Server::SpawnBall() {
	// cout << "SPAWNED BALL" << endl;
	if (balls.size() < MAX_BALLS) {
		float posX = (float)(std::rand() % WORLD_WIDTH);
		float posY = (float)(std::rand() % WORLD_HEIGHT);
		balls.push_back(new Ball(posX, posY, BALLSIZE, 0.f, BallType::FOOD));	
		return true;
	}
	return false;
}

void Server::SendWorld(CPeerENet* peer) {
	// Snapshot of all entities (reliable)
	CBuffer* buffer = new CBuffer();
	AgarioSerialization::SerializeWorld(*buffer, balls, MsgType::WORLD);
    pServer->SendData(peer, buffer->GetBytes(), buffer->GetSize(), 1, true);
	delete buffer;
}

void Server::UpdateBalls() {
	std::vector<Ball*> deletedBalls;
	// Collisions
	for (auto ball1 : balls) {
		for (auto ball2 : balls) {
			if (ball1 != ball2)	{
				float dist = Distance(ball1->posX, ball1->posY, ball2->posX, ball2->posY);
				// Radius is sprite size / 2
				if (dist + ball1->size/2 <= ball2->size/2) {
						// One is a player
					if (ball1->playerID != 0 || ball2->playerID != 0) {
						// Mark food as dead
						if (ball1->playerID == 0) {
							deletedBalls.push_back(ball1);
							ball2->size += 10;
						}
						else if (ball2->playerID == 0) {
							deletedBalls.push_back(ball2);
							ball2->size += 10;
						}
						// Both are players
						else if (ball1->playerID != 0 && ball2->playerID != 0) {
							if (ball1->size > ball2->size) {
								deletedBalls.push_back(ball2);
								ball1->size += 10;
							}
							else {
								deletedBalls.push_back(ball1);
								ball2->size += 10;
							}
						}
					}
				}
			}
		}
	}

	// Remove dead
	for (auto ball : deletedBalls) {
		balls.erase(std::remove_if(balls.begin(), balls.end(),
			[ball](Ball* b) { return b == ball; }), balls.end());
	}
	deletedBalls.clear();
}

void Server::UpdateClients() {
	if (elapsedUpdateT >= updateRateT) {
		elapsedUpdateT = 0;

		// Snapshot of all entities (non reliable)
		CBuffer* buffer = new CBuffer();
		AgarioSerialization::SerializeWorld(*buffer, balls, MsgType::UPDATE);
		pServer->SendAll(buffer->GetBytes(), buffer->GetSize(), 0, false);
		delete buffer;
	}
	else elapsedUpdateT += DELTA_TIME;
}

void Server::AddClient(CPeerENet* peer) {
	Ball* ball = new Ball(STARTPOSX, STARTPOSY, BALLSIZE + 20, INIT_PLAYER_SPEED,
		BallType::PLAYER);

	// Each packet has owner Peer (UDP)
	size_t playerID = clients.size() + 1;
	ball->playerID = playerID;
	clients.push_back(Client(peer, ball, playerID));
	balls.push_back(ball);
	
	// Send snapshot of the world to the client
	SendID(peer, playerID);
	SendWorld(peer);
}

void Server::RemoveClient(CPeerENet* peer) {
	auto client = std::find_if(clients.begin(), clients.end(),
		[peer](Client &c) { return c.peer == peer; });
	
	// Client was already deleted
	if (client == clients.end()) return;

	size_t ID = client->ID;
	auto ball = std::find_if(balls.begin(), balls.end(),
		[ID](Ball *b) { return b->playerID == ID; });
	
	// Ball was already deleted
	if (ball == balls.end()) return;

	balls.erase(ball);
	clients.erase(client);
}

void Server::MovePlayerToMousePos(CBuffer* buffer) {
	size_t ID   = 0;
	float destX = 0.f;
	float destY = 0.f;
	AgarioSerialization::DeserializeMousePos(*buffer, ID, destX, destY);

	if (ID != 0) {
		// Find that player
		auto it = std::find_if(clients.begin(), clients.end(),
			[ID](Client &c) { return c.ID == ID; });
		float posX = it->ball->posX;
		float posY = it->ball->posY;

		// Move him
		if (Distance(destX, destY, posX, posY) > 10.f) {
			// Normalize direction
			float velX = destX - posX;
			float velY = destY - posY;
			float length = sqrtf(velX * velX + velY * velY);
			velX /= length;
			velY /= length;
			// Speed
			velX += velX * it->ball->speed;
			velY += velY * it->ball->speed;

			it->ball->posX += velX;
			it->ball->posY += velY;
		}
	}
}

Server::Client::Client(CPeerENet* _peer, Ball* _ball, size_t _ID) :
	peer(_peer),
	ball(_ball),
	ID(_ID)
{}

void Server::SendID(CPeerENet* peer, size_t ID) {
	CBuffer* buffer = new CBuffer();
	AgarioSerialization::SerializeID(*buffer, ID);
	pServer->SendData(peer, buffer->GetBytes(), buffer->GetSize(), 1, true);
	delete buffer;
}