#pragma once

#include "buffer.h"
#include <string>

class AgarioSerialization
{
public:
	AgarioSerialization();
	~AgarioSerialization();

	static void SerializeMousePos(CBuffer &buffer, size_t ID, float posX, float posY);
	static void DeserializeMousePos(CBuffer &buffer, size_t &outID, float &posX, float &posY);
};

