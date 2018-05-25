#pragma once

#define BALLSIZE 30
#define STARTPOSX 320
#define STARTPOSY 240

#include <math.h>

// =========================================================================
//                Common data for both Server and Client
// =========================================================================

enum class BallType { FOOD, PLAYER };

struct Ball {
	Ball(float _x, float _y, float _rad, float _speed, BallType _type) :
		posX(_x), posY(_y), radius(_rad), speed(_speed), type(_type),
		playerID(0)
	{}

	bool operator==(const Ball& other) { 
		if (posX == other.posX && posY == other.posY && radius == other.radius && speed == other.speed && type == other.type && playerID == other.playerID)
			return true;
		else return false;
	}
	
	float posX;
	float posY;
	float radius;
	float speed;
	BallType type;
	size_t playerID;
};

// Identify byte buffers
enum class MsgType { WORLD, UPDATE, MOVE, ID };

inline float Distance(float aX, float aY, float bX, float bY) {
	float vX = bX - aX;
	float vY = bY - aY;

	float dist = sqrtf(vX * vX + vY * vY);
	return dist;
}

inline float Max(float a, float b) {
	return a > b ? a : b;
}

inline float Min(float a, float b) {
	return a < b ? a : b;
}