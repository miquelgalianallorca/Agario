#include "AgarioSerialization.h"
#include "AgarioData.h"

AgarioSerialization::AgarioSerialization()
{
}


AgarioSerialization::~AgarioSerialization()
{
}

void AgarioSerialization::SerializeMousePos(CBuffer &buffer, size_t ID, float posX, float posY) {
	MsgType msgType = MsgType::MOVE;
	buffer.Write(&msgType, sizeof(MsgType));

	buffer.Write(&ID, sizeof(ID));
	buffer.Write(&posX, sizeof(float));
	buffer.Write(&posY, sizeof(float));

	buffer.GotoStart();
}

void AgarioSerialization::DeserializeMousePos(CBuffer &buffer, size_t &outID, float &posX, float &posY) {
	buffer.Read(&outID, sizeof(size_t));
	//if (ID != 0)
	//{
	//	float destX = 0.f;
	//	float destY = 0.f;
	//	buffer->Read(&destX, sizeof(float));
	//	buffer->Read(&destY, sizeof(float));

	//	// Find that player
	//	auto it = std::find_if(clients.begin(), clients.end(),
	//		[ID](Client &c) { return c.ID == ID; });
	//	float posX = it->ball->posX;
	//	float posY = it->ball->posY;

	//	// Move him
	//	if (Distance(destX, destY, posX, posY) > 10.f)
	//	{
	//		// Normalize direction
	//		float velX = destX - posX;
	//		float velY = destY - posY;
	//		float length = sqrtf(velX * velX + velY * velY);
	//		velX /= length;
	//		velY /= length;
	//		// Speed
	//		velX += velX * it->ball->speed;
	//		velY += velY * it->ball->speed;

	//		it->ball->posX += velX;
	//		it->ball->posY += velY;
	//	}
	//}
}