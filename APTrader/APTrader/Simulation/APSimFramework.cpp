#include "APSimFramework.h"
#include "../APGlobalConfig.h"
#include "APSimAccount.h"
#include "APSimMarketQuotations.h"
#include "../APStrategyManager.h"
#include "../APPositionManager.h"
#include "../APStrategyFactory.h"
#include "../Utils/APJsonReader.h"
#include "../APTradeManager.h"
#include "APSimFuturesTrade.h"
#include "APSimTradeSystem.h"
#include "../Utils/APLog.h"

std::string simConfigFile = "Data/Sim/Sim.cfg";


APSimFramework::APSimFramework()
{
	//
}


APSimFramework::~APSimFramework()
{
}

void APSimFramework::init() {
	initFromCfg();

	APStrategyFactory::init();
	APSimAccount::getInstance()->init();
	APGlobalConfig::getInstance()->init();
	APSimMarketQuotations::getInstance()->init();

	APTradeManager::getInstance()->setTradeInstance(generateSimTradeInstance());

	APSimTradeSystem::getInstance()->init();

	APPositionManager::getInstance();
	APStrategyManager::getInstance();
	
	postInit();
}

void APSimFramework::postInit() {
	for (int i = 0; i < m_simStrategies.size(); i++) {		
		APStrategyManager::getInstance()->runStrategy("Data/" + m_simStrategies[i]);
	}
}

APTrade * APSimFramework::generateSimTradeInstance()
{
	return new APSimFuturesTrade();
}

void APSimFramework::update() {
	APSimMarketQuotations::getInstance()->update();
	APPositionManager::getInstance()->update();
	APStrategyManager::getInstance()->update();
	APSimTradeSystem::getInstance()->update();

	if (m_mode == SM_Discrete) {
		m_discreteCount++;

		if (m_discreteCount >= m_discreteTimes) {
			m_finished = true;
		}
	}
}

void APSimFramework::exit()
{
	double totalAssets = APSimAccount::getInstance()->getTotalAssetsValue();
	double curFund = APSimAccount::getInstance()->getFund();
	double simAssetsVal = APSimAccount::getInstance()->calcAssetsProfitAndLoss();
	APLogger->log("==== Settlement total: %f, fund: %f, assets:%f.", totalAssets, curFund, simAssetsVal);
}

void APSimFramework::initFromCfg()
{
	APJsonReader jr;
	jr.initWithFile(simConfigFile);
	std::string mode = jr.getStrValue("SimMode");
	if (mode == "Discrete") {
		m_mode = SM_Discrete;
	}
	else if (mode == "Continuous") {
		m_mode = SM_Continuous;
	}

	m_discreteTimes = jr.getIntValue("DiscreteTimes");

	int strategiesCount = jr.getArraySize("SimStrategies");
	for (int i = 0; i < strategiesCount; i++) {
		m_simStrategies.push_back(jr.getArrayElement("SimStrategies", i));
	}
	
	m_discreteCount = 0;
}
