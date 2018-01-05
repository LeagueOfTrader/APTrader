#pragma once
#include "../Common/Singleton.h"
#include <string>


class APRedisAgent : public Singleton<APRedisAgent>
{
public:
	void init();

	void setValue(std::string key, std::string field, std::string value);
	std::string getValue(std::string key, std::string field);

private:
	//
};

