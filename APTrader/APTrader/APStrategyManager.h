#pragma once
#include "Common/Singleton.h"
#include "Common/InitializableObject.h"
#include "Common/Ticker.h"
#include <map>
#include <string>

class APStrategy;
class APIntAccumulator;

class APStrategyManager : public InitializableObject, public Ticker, public Singleton<APStrategyManager>
{
public:
	APStrategyManager();
	~APStrategyManager();

	virtual void init();
	virtual void update();
	virtual void exit();

	APStrategy* loadStrategy(std::string strategyName);
	void unloadStrategy(std::string strategyName);
	bool runStrategy(std::string strategyName);
	bool stopStrategy(std::string strategyName);

//protected:
	APStrategy* createStrategy(std::string strategyFile);

	APStrategy* getStrategy(std::string strategyName);

	void runAllStrategies();
	void stopAllStrategies();

private:
	std::string makeUpStrategyFileName(std::string strategyName);

private:
	std::map<std::string, APStrategy*> m_strategies;
	APIntAccumulator* m_idAccumulator;
};

