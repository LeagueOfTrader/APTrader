#pragma once
#include <string>
#include <map>

class APStrategy;

typedef APStrategy*(*StrategyCreateMethod)();

class APStrategyFactory
{
public:
	static void init();
	static APStrategy* createStrategy(std::string name);

private:
	static std::map<std::string, StrategyCreateMethod> m_createMethods;
};

