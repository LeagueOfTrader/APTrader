#include "APStrategyManager.h"
#include "APStrategy.h"
#include "APStrategyFactory.h"
#include "Utils/APJsonReader.h"
#include "Utils/APIntAccumulator.h"


std::string strategyStartupFile = "Data/Strategy/startup.cfg";
std::string strategyPath = "Data/Strategy/";

APStrategyManager::APStrategyManager()
{
	m_idAccumulator = new APIntAccumulator();
}

APStrategyManager::~APStrategyManager()
{
	delete m_idAccumulator;
}

void APStrategyManager::init()
{
	APJsonReader jr;
	jr.initWithFile(strategyStartupFile);

	int count = jr.getArraySize("Strategies");
	for (int i = 0; i < count; i++) {
		std::string strategyName = jr.getArrayStrValue("Strategies", i);
		APStrategy* strategy = loadStrategy(strategyName);
		// ---- run after load
		runStrategy(strategyName);
	}
}

void APStrategyManager::update()
{
	std::map<std::string, APStrategy*>::iterator it = m_strategies.begin();
	for (; it != m_strategies.end(); it++) {
		APStrategy* strategy = it->second;
		strategy->update();
	}
}

void APStrategyManager::exit()
{
	std::map<std::string, APStrategy*>::iterator it = m_strategies.begin();
	for (; it != m_strategies.end(); it++) {
		APStrategy* strategy = it->second;
		strategy->exit();
	}
}

APStrategy * APStrategyManager::loadStrategy(std::string strategyName)
{
	if (m_strategies.find(strategyName) != m_strategies.end()) {
		return m_strategies[strategyName];
	}

	std::string filename = makeUpStrategyFileName(strategyName);
	APStrategy* strategy = createStrategy(filename);
	m_strategies[strategyName] = strategy;
	return strategy;
}

void APStrategyManager::unloadStrategy(std::string strategyName)
{
	if (m_strategies.find(strategyName) == m_strategies.end()) {
		return;
	}

	m_strategies[strategyName]->exit();
	m_strategies.erase(strategyName);
}

bool APStrategyManager::runStrategy(std::string strategyName)
{
	if (m_strategies.find(strategyName) == m_strategies.end()) {
		return false;
	}

	m_strategies[strategyName]->setWork(true);
	return true;
}

bool APStrategyManager::stopStrategy(std::string strategyName)
{
	if (m_strategies.find(strategyName) == m_strategies.end()) {
		return false;
	}

	m_strategies[strategyName]->setWork(false);
	return true;
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

std::string APStrategyManager::makeUpStrategyFileName(std::string strategyName)
{
	return strategyPath + strategyName + ".cfg";
}
