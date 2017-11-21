#pragma once
#include <string>
#include "../3rdParty/jsoncpp/include/json/reader.h"


class APJsonReader
{
public:
	APJsonReader();
	~APJsonReader();

	void initWithFile(std::string filename);
	void initWithString(std::string value);

	std::string getObjValue(std::string key);
	std::string getStrValue(std::string key);
	double getDoubleValue(std::string key);
	int getIntValue(std::string key);
	bool getBoolValue(std::string key);
	int getArraySize(std::string key);
	std::string getArrayElement(std::string key, int index);
	bool hasMember(std::string key);

private:
	Json::Value m_root;
	bool m_inited;
};

