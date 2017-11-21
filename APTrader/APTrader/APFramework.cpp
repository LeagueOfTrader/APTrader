#include "APFramework.h"
#include "APGlobalConfig.h"
#include "APAccountAssets.h"
#include "APMarketQuotationsManager.h"
#include "APStrategyManager.h"
#include "APPositionManager.h"
#include "APTradeManager.h"

APFramework::APFramework()
{
	m_finished = false;
}


APFramework::~APFramework()
{
}

void APFramework::init()
{
	APGlobalConfig::getInstance()->init();
	APAccountAssets::getInstance()->init();
	APMarketQuotationsManager::getInstance()->init();
	APTradeManager::getInstance()->init();
}

void APFramework::update()
{
	APMarketQuotationsManager::getInstance()->update();
	APPositionManager::getInstance()->update();
	APStrategyManager::getInstance()->update();
}

void APFramework::exit()
{
}

bool APFramework::finished()
{
	return m_finished;
}
