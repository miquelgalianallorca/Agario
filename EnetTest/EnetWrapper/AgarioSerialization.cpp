#include "AgarioSerialization.h"
#include "AgarioData.h"
#include "buffer.h"

void AgarioSerialization::SerializeMousePos(CBuffer &buffer, size_t ID, float posX, float posY) {
	MsgType msgType = MsgType::MOVE;
	buffer.Write(&msgType, sizeof(MsgType));

	buffer.Write(&ID, sizeof(ID));
	buffer.Write(&posX, sizeof(float));
	buffer.Write(&posY, sizeof(float));

	buffer.GotoStart();
}

void AgarioSerialization::DeserializeMousePos(CBuffer &buffer, size_t &outID, float &outPosX, float &outPosY) {
	buffer.Read(&outID, sizeof(size_t));
	if (outID != 0)
	{
		buffer.Read(&outPosX, sizeof(float));
		buffer.Read(&outPosY, sizeof(float));
	}
}

void AgarioSerialization::SerializeID(CBuffer &outBuffer, size_t ID) {
	MsgType msgType = MsgType::ID;
	outBuffer.Write(&msgType, sizeof(MsgType));
	outBuffer.Write(&ID, sizeof(size_t));
}

void AgarioSerialization::DeserializeID(CBuffer &buffer, size_t &outID) {
	buffer.Read(&outID, sizeof(size_t));
}

void AgarioSerialization::SerializeWorld(CBuffer& outBuffer, std::vector<Ball>& balls, MsgType msgType) {
	// Message type
	outBuffer.Write(&msgType, sizeof(MsgType));

	// Balls
	size_t numBalls = balls.size();
	outBuffer.Write(&numBalls, sizeof(size_t));
	for (auto ball : balls)
		outBuffer.Write(&ball, sizeof(Ball));

	outBuffer.GotoStart();
}

void AgarioSerialization::DeserializeWorld(CBuffer &buffer, std::vector<Ball>& outBalls) {
	size_t numBalls;
	buffer.Read(&numBalls, sizeof(size_t));
	for (size_t i = 0; i<numBalls; ++i)
	{
		Ball ball(0, 0, 0, 0.f, BallType::FOOD);
		buffer.Read(&ball, sizeof(Ball));
		outBalls.push_back(ball);
	}
}

void AgarioSerialization::SerializeWorld(CBuffer& outBuffer, std::vector<Ball*>& balls, MsgType msgType)
{
	// Message type
	outBuffer.Write(&msgType, sizeof(MsgType));

	// Balls
	size_t numBalls = balls.size();
	outBuffer.Write(&numBalls, sizeof(size_t));
	for (auto ball : balls)
		outBuffer.Write(ball, sizeof(Ball));

	outBuffer.GotoStart();
}

void AgarioSerialization::DeserializeWorld(CBuffer &buffer, std::vector<Ball*>& outBalls)
{
	size_t numBalls;
	buffer.Read(&numBalls, sizeof(size_t));
	for (size_t i = 0; i<numBalls; ++i)
	{
		Ball* ball = new Ball(0, 0, 0, 0.f, BallType::FOOD);
		buffer.Read(ball, sizeof(Ball));
		outBalls.push_back(ball);
	}
}