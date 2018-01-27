#pragma once
#include <string>
#include <map>
#include "Common/InitializableObject.h"
#include "Common/Singleton.h"

class APStrategy;

typedef APStrategy*(*StrategyCreateMethod)();

class APStrategyFactory : public InitializableObject, public Singleton<APStrategyFactory>
{
public:
	virtual void init();
	APStrategy* createStrategy(std::string name);
	void registerCreator(std::string name, StrategyCreateMethod function);

private:
	std::map<std::string, StrategyCreateMethod> m_createMethods;
};

