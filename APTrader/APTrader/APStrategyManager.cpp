#include "APStrategyManager.h"
#include "APStrategy.h"
#include "APStrategyFactory.h"
#include "Utils/APJsonReader.h"
#include "Utils/APIntAccumulator.h"

APStrategyManager::APStrategyManager()
{
	m_idAccumulator = new APIntAccumulator();
}


APStrategyManager::~APStrategyManager()
{
	delete m_idAccumulator;
}

void APStrategyManager::update()
{
	std::map<std::string, APStrategy*>::iterator it = m_strategies.begin();
	for (; it != m_strategies.end(); it++) {
		APStrategy* strategy = it->second;
		strategy->update();
	}
}

APStrategy * APStrategyManager::runStrategy(std::string strategyFile)
{
	if (m_strategies.find(strategyFile) != m_strategies.end()) {
		return m_strategies[strategyFile];
	}

	APStrategy* strategy = createStrategy(strategyFile);
	m_strategies[strategyFile] = strategy;
	return strategy;
}

APStrategy * APStrategyManager::createStrategy(std::string strategyFile)
{
	APJsonReader jr;
	jr.initWithFile(strategyFile);
	std::string strategyType = jr.getStrValue("StrategyType");
	APStrategy* strategy = APStrategyFactory::createStrategy(strategyType);
	std::string strategyInfo = jr.getObjValue("StrategyInfo");
	if(jr.hasMember("Priotiry")){
		UINT priority = jr.getIntValue("Priority");
		strategy->setPriority(priority);
	}
	strategy->init(strategyInfo);
	strategy->setID(m_idAccumulator->generateID());
	return strategy;
}
