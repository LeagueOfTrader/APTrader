#include "APTestFramework.h"
#include "../Utils/APRedisAgent.h"
#include "../APAccountInfo.h"
#include "../APTradeManager.h"
#include "../APStrategyManager.h"
#include "../APMarketDataManager.h"
#include "../APGlobalConfig.h"
#include "../Input/APInputSystem.h"

#ifdef USE_CTP
#include "../Impl/CTP/APFuturesCTPMDAgent.h"
#include "../Impl/CTP/APFuturesCTPTraderAgent.h"

void onTestCTPInited()
{
	APTestFramework::getInstance()->onCTPApiInited();
}


APTestFramework::APTestFramework()
{
	m_finished = false;
	m_inited = false;
	m_ctpCount = 0;
}

APTestFramework::~APTestFramework()
{
	//APInputSystem::getInstance()->exit();
}

void APTestFramework::init()
{
	APGlobalConfig::getInstance()->init();

	initCTP();
	//APInputSystem::getInstance()->init();
	//setInited();
}

void APTestFramework::update()
{
	//APInputSystem::getInstance()->update();
}

void APTestFramework::exit()
{
	APStrategyManager::getInstance()->exit();
	APTradeManager::getInstance()->exit();

	m_exited = true;
}

bool APTestFramework::inited()
{
	return m_inited;
}

bool APTestFramework::finished()
{
	return m_finished;
}

void APTestFramework::initLocalSystem()
{
	APMarketDataMgr->init();
	//APRedisAgent::getInstance()->init();
	//APAccountInfo::getInstance()->init();
	APTradeManager::getInstance()->init();
	//
	//APStrategyManager::getInstance()->init();
	//APAccountInfo::getInstance()->beginVerify();

	setInited();
}


void APTestFramework::onCTPApiInited()
{
	m_ctpCount++;

	if (m_ctpCount >= 2) {
		initLocalSystem();
	}
}

void APTestFramework::initCTP()
{
	APFuturesCTPMDAgent::getInstance()->registerInitCallback(onTestCTPInited);
	APFuturesCTPMDAgent::getInstance()->init();

	APFuturesCTPTraderAgent::getInstance()->registerInitCallback(onTestCTPInited);
	APFuturesCTPTraderAgent::getInstance()->init();
}

#endif




