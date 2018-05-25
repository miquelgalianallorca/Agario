#pragma once

#include <vector>

struct Ball;
class CBuffer;
enum class MsgType;

class AgarioSerialization
{
public:
	AgarioSerialization()  {}
	~AgarioSerialization() {}

	static void SerializeMousePos(CBuffer &outBuffer, size_t ID, float posX, float posY);
	static void DeserializeMousePos(CBuffer &buffer, size_t &outID, float &outPosX, float &outPosY);

	static void SerializeID(CBuffer& outBuffer, size_t ID);
	static void DeserializeID(CBuffer& buffer, size_t& outID);

	static void SerializeWorld(CBuffer& outBuffer, std::vector<Ball>& balls, MsgType msgType);
	static void DeserializeWorld(CBuffer& buffer, std::vector<Ball>& outBalls);
	static void SerializeWorld(CBuffer& outBuffer, std::vector<Ball*>& balls, MsgType msgType);
	static void DeserializeWorld(CBuffer& buffer, std::vector<Ball*>& outBalls);
};

