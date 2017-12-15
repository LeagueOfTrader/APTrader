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
	
	std::string getArrayObjValue(std::string key, int index);
	std::string getArrayStrValue(std::string key, int index);
	double getArrayDoubleValue(std::string key, int index);
	int getArrayIntValue(std::string key, int index);
	bool getArrayBoolValue(std::string key, int index);

	std::string getArrayFieldStrValue(std::string key, std::string field, int index);
	double getArrayFieldDoubleValue(std::string key, std::string field, int index);
	int getArrayFieldIntValue(std::string key, std::string field, int index);
	bool getArrayFieldBoolValue(std::string key, std::string field, int index);
	
	//std::string getArrayElement(std::string key, int index);
	
	//std::string getArrayElementField(std::string key, std::string field, int index);
	bool hasMember(std::string key);

	static std::string getStrValue(Json::Value& val);
	static double getDoubleValue(Json::Value& val);
	static int getIntValue(Json::Value& val);
	static bool getBoolValue(Json::Value& val);

private:
	Json::Value& getArrayObject(std::string key, int index);

private:
	Json::Value m_root;
	bool m_inited;
};

