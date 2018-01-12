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

	void init();
	void update();
	void exit();

	APStrategy* runStrategy(std::string strategyName);
	void stopStrategy(std::string strategyName);

//protected:
	APStrategy* createStrategy(std::string strategyFile);

private:
	std::string makeUpStrategyFileName(std::string strategyName);

private:
	std::map<std::string, APStrategy*> m_strategies;
	APIntAccumulator* m_idAccumulator;
};

