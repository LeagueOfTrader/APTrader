#pragma once
#include "Common/Singleton.h"
#include <map>
#include <string>

class APStrategy;
class APIntAccumulator;

class APStrategyManager : public Singleton<APStrategyManager>
{
public:
	APStrategyManager();
	~APStrategyManager();

	void update();
	APStrategy* runStrategy(std::string strategyFile);

//protected:
	APStrategy* createStrategy(std::string strategyFile);

private:
	std::map<std::string, APStrategy*> m_strategies;
	APIntAccumulator* m_idAccumulator;
};

