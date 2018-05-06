#pragma once

// =========================================================================
//                Common data for both Server and Client
// =========================================================================

struct Ball {
	Ball(float _x, float _y, float _rad) : posX(_x), posY(_y), radius(_rad) {}
	float posX;
	float posY;
	float radius;
};

// Messages ================================================================
enum class MsgType { WORLD, UPDATE };

//struct Msg {};
//
//struct MsgWorld : public Msg {
//	std::vector<Ball> balls;
//};
// =========================================================================
