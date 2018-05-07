#pragma once

#define BALLSIZE 30
#define STARTPOSX 320
#define STARTPOSY 240

// =========================================================================
//                Common data for both Server and Client
// =========================================================================

enum class BallType { FOOD, PLAYER };

struct Ball {
	Ball(float _x, float _y, float _rad, BallType _type) :
		posX(_x), posY(_y), radius(_rad), type(_type) {}
	
	float posX;
	float posY;
	float radius;
	BallType type;
};

// Identify byte buffers
enum class MsgType { WORLD, UPDATE, MOVE };
