#pragma once
#include <string>
#include <map>
#include "../APTypes.h"
#include <vector>

class APInputCommandParser
{
public:
	static void parseCommand(std::string cmd);
	static void init();

private:
	static bool processCommand(std::vector<std::string> param);

private:
	static std::map<std::string, UINT> ms_commandMap;
};

