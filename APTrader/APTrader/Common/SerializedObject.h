#pragma once
#include <string>

class SerializedObject
{
public:
	virtual void serialize(std::string filename) = 0;
	virtual void deserialize(std::string filename) = 0;
};

