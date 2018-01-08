#include "APRedisSerializedObject.h"
#include "APRedisAgent.h"

void APRedisSerializedObject::archive(std::string key)
{
	std::string serializedData = serialize();
	APRedisAgent::getInstance()->writeAsync(key, serializedData);
}

void APRedisSerializedObject::unarchive(std::string key) 
{
	std::string data = APRedisAgent::getInstance()->read(key);
	deserialize(data);
}
