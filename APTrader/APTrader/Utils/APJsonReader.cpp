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

std::string APJsonReader::getArrayElement(std::string key, int index) {

	if (hasMember(key)) {
		return m_root[key][index].asString();
	}

	return "";
}

bool APJsonReader::hasMember(std::string key)
{
	if (m_inited) {
		return m_root.isMember(key);
	}

	return false;
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
