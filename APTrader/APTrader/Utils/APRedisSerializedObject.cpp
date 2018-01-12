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

void APRedisSerializedObject::load()
{
	std::string redisKey = generateRedisKey();
	unarchive(redisKey);
}

void APRedisSerializedObject::save()
{
	std::string redisKey = generateRedisKey();
	archive(redisKey);
}

std::string APRedisSerializedObject::generateRedisKey()
{
	return "DEFAULT";
}
