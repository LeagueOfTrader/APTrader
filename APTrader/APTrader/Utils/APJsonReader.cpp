#include "APJsonReader.h"
#include "writer.h"
#include <fstream>

APJsonReader::APJsonReader()
{
	m_inited = false;
}


APJsonReader::~APJsonReader()
{
}

std::string APJsonReader::getObjValue(std::string key)
{
	if (hasMember(key)) {
		Json::FastWriter fw;
		return fw.write(m_root[key]);
	}

	return "";
}

std::string APJsonReader::getStrValue(std::string key)
{
	if (hasMember(key)) {
		return m_root[key].asString();
	}

	return "";
}

double APJsonReader::getDoubleValue(std::string key)
{
	if (hasMember(key)) {
		return m_root[key].asDouble();
	}

	return 0.0;
}

int APJsonReader::getIntValue(std::string key)
{
	if (hasMember(key)) {
		return m_root[key].asInt();
	}

	return 0;
}

bool APJsonReader::getBoolValue(std::string key)
{
	if (hasMember(key)) {
		return m_root[key].asBool();
	}

	return false;
}

int APJsonReader::getArraySize(std::string key)
{
	if (hasMember(key)) {
		return m_root[key].size();
	}
	return 0;
}

std::string APJsonReader::getArrayObjValue(std::string key, int index)
{
	if (hasMember(key)) {
		Json::FastWriter fw;
		return fw.write(m_root[key][index]);
	}

	return "";
}

std::string APJsonReader::getArrayStrValue(std::string key, int index)
{
	if (hasMember(key)) {
		return m_root[key][index].asString();
	}

	return "";
}

double APJsonReader::getArrayDoubleValue(std::string key, int index)
{
	if (hasMember(key)) {
		return m_root[key][index].asDouble();
	}

	return 0.0;
}

int APJsonReader::getArrayIntValue(std::string key, int index)
{
	if (hasMember(key)) {
		return m_root[key][index].asInt();
	}

	return 0;
}

bool APJsonReader::getArrayBoolValue(std::string key, int index)
{
	if (hasMember(key)) {
		return m_root[key][index].asBool();
	}

	return false;
}

std::string APJsonReader::getArrayFieldStrValue(std::string key, std::string field, int index)
{
	if (hasMember(key)) {
		return m_root[key][index][field].asString();
	}

	return "";
}

double APJsonReader::getArrayFieldDoubleValue(std::string key, std::string field, int index)
{
	if (hasMember(key)) {
		return m_root[key][index][field].asDouble();
	}

	return 0.0;
}

int APJsonReader::getArrayFieldIntValue(std::string key, std::string field, int index)
{
	if (hasMember(key)) {
		return m_root[key][index][field].asInt();
	}

	return 0;
}

bool APJsonReader::getArrayFieldBoolValue(std::string key, std::string field, int index)
{
	if (hasMember(key)) {
		return m_root[key][index][field].asBool();
	}

	return false;
}

//std::string APJsonReader::getArrayElement(std::string key, int index) {
//
//	if (hasMember(key)) {
//		return m_root[key][index].asString();
//	}
//
//	return "";
//}

Json::Value & APJsonReader::getArrayObject(std::string key, int index)
{
	return m_root[key][index];
}

//std::string APJsonReader::getArrayElementField(std::string key, std::string field, int index) {
//
//	if (hasMember(key)) {
//		return m_root[key][index][field].asString();
//	}
//
//	return "";
//}

bool APJsonReader::hasMember(std::string key)
{
	if (m_inited) {
		return m_root.isMember(key);
	}

	return false;
}

std::string APJsonReader::getStrValue(Json::Value & val)
{
	return val.asString();
}

double APJsonReader::getDoubleValue(Json::Value & val)
{
	return val.asDouble();
}

int APJsonReader::getIntValue(Json::Value & val)
{
	return val.asInt();
}

bool APJsonReader::getBoolValue(Json::Value & val)
{
	return val.asBool();
}

void APJsonReader::initWithFile(std::string filename)
{
	Json::Reader reader;

	//std::fstream file;
	//file.open(filename, std::ios::in);
	//if (!file) {
	//	return;
	//}

	std::ifstream ifs;
	ifs.open(filename, std::ios::binary);
	if (reader.parse(ifs, m_root)) {
		m_inited = true;
	}
	ifs.close();
}

void APJsonReader::initWithString(std::string value)
{
	Json::Reader reader;
	if (reader.parse(value, m_root)) {
		m_inited = true;
	}
}
