#include "APFuturesFramework.h"
#include "../Utils/APRedisAgent.h"
#include "../APAccountInfo.h"
#include "../APTradeManager.h"
#include "../APStrategyManager.h"
#include "../APMarketDataManager.h"
#include "../APGlobalConfig.h"
#include "../APSystemSetting.h"
#include <windows.h>
#include "../APStrategyFactory.h"
#include "../Input/APInputSystem.h"
#include "../APPositionManager.h"
#include "../System/APTimerEventManager.h"
#include "../Utils/APTimeUtility.h"

#ifdef USE_CTP
#include "../Impl/CTP/APFuturesCTPMDAgent.h"
#include "../Impl/CTP/APFuturesCTPTraderAgent.h"
#endif

//#ifndef SIMNOW
//#define SIMNOW
//#endif
//
//std::string mdflowpath = "Data/CTP/mdflow/md.con";
//std::string ctpCfgFile = "Data/CTP/CTPAccount.cfg";
//
//#ifdef SIMNOW
//std::string ctpParamFile = "Data/CTP/CTPTestParam.cfg";
//#else
//std::string ctpParamFile = "Data/CTP/CTPParam.cfg";
//#endif
#ifdef USE_CTP
void onCTPTradeInited()
{
	APFuturesFramework::getInstance()->onCTPApiInited(CTP_Flag_Trade);
}

void onCTPMdInited()
{
	APFuturesFramework::getInstance()->onCTPApiInited(CTP_Flag_Md);
}
#endif // USE_CTP

void onNewTransactionDay() {
	APFuturesFramework::getInstance()->onStartTransactionDay();
}

void onCloseTransactionDay() {
	APFuturesFramework::getInstance()->onEndTransactionDay();
}

APFuturesFramework::APFuturesFramework()
{	
	m_finished = false;

#ifdef USE_CTP
	m_initFlag = CTP_Flag_None;
#endif
}

APFuturesFramework::~APFuturesFramework()
{
	if (APGlobalConfig::getInstance()->useInput()) {
		APInputSystem::getInstance()->exit();
	}
}

//void APFuturesFramework::init() 
//{
//	APGlobalConfig::getInstance()->init();
//	APRedisAgent::getInstance()->init();
//	APSystemSetting::getInstance()->init();
//
//#ifdef USE_CTP
//	initCTP();
//#else
//	initLocalSystem();
//#endif // USE_CTP
//
//}

//void APFuturesFramework::update(float deltaTime)
//{
//	APMarketDataMgr->update(deltaTime);
//	APAccountInfo::getInstance()->update();
//	APStrategyManager::getInstance()->update();
//}
//
//void APFuturesFramework::exit()
//{
//	APStrategyManager::getInstance()->exit();
//	APTradeManager::getInstance()->exit();
//}

void APFuturesFramework::ready()
{
	addPreInit(APGlobalConfig::getInstance());
	addPreInit(APRedisAgent::getInstance());
	addPreInit(APSystemSetting::getInstance());

	addInit(APMarketDataMgr);
	addInit(APStrategyFactory::getInstance());
	addInit(APAccountInfo::getInstance());
	addInit(APTradeManager::getInstance());

	addPostInit(APStrategyManager::getInstance());

	addTicker(APMarketDataMgr);
	//addTicker(APAccountInfo::getInstance());
	addTicker(APStrategyManager::getInstance());

	if (APGlobalConfig::getInstance()->useInput()) {
		addTicker(APInputSystem::getInstance());
	}
}

void APFuturesFramework::start()
{
	//APGlobalConfig::getInstance()->init();
	//APRedisAgent::getInstance()->init();
	//APSystemSetting::getInstance()->init();

	preInit();

#ifdef USE_CTP
	initCTP();
#else
	initLocalSystem();
#endif

	//time_t curTime = APTimeUtility::getTimestamp();
	std::string curDateTime = APTimeUtility::getDateTime();
	std::string curDate = APTimeUtility::getDate();
	std::string nextDate = APTimeUtility::getNextTransactionDay();
	std::string nextTransactionDateTime = "";	
	std::string curEndTransactionDateTime = "";

	UINT curHour = APTimeUtility::getHourInDateTime(curDateTime);
	if (curHour <= 15) {
		nextTransactionDateTime = APTimeUtility::makeUpDateTime(curDate, 21);
		curEndTransactionDateTime = APTimeUtility::makeUpDateTime(curDate, 15);
	}
	else
	{
		if (curHour >= 21) {
			nextTransactionDateTime = APTimeUtility::makeUpDateTime(nextDate, 21);
		}
		else {
			nextTransactionDateTime = APTimeUtility::makeUpDateTime(curDate, 21);
		}
		curEndTransactionDateTime = APTimeUtility::makeUpDateTime(nextDate, 15);
	}

	UINT secToNext = APTimeUtility::calcDeltaSeconds(curDateTime, nextTransactionDateTime);
	UINT secToEnd = APTimeUtility::calcDeltaSeconds(curDateTime, curEndTransactionDateTime);

	APTimerEventManager::getInstance()->registerEvent(onNewTransactionDay, secToNext * 1000, 24 * 60 * 60 * 1000);
	APTimerEventManager::getInstance()->registerEvent(onCloseTransactionDay, secToEnd * 1000, 24 * 60 * 60 * 1000);
}

bool APFuturesFramework::finished()
{
	return m_finished;
}

bool APFuturesFramework::inited()
{
	return m_inited;
}

void APFuturesFramework::initLocalSystem()
{
	APMarketDataMgr->init();	
	
	APTradeManager::getInstance()->init();
	APStrategyFactory::getInstance()->init();
	APTrade* trader = APTradeManager::getInstance()->getTradeInstance();
	if (trader == NULL) {
		return;
	}

	while (!trader->inited() ) {
		Sleep(1000);
	}

	APAccountInfo::getInstance()->init();

	while (!APAccountInfo::getInstance()->inited()) {
		Sleep(1000);
	}

	APStrategyManager::getInstance()->init();

	//init();

	//long waitInterval = APSystemSetting::getInstance()->getInitializeStateWaitInterval();
	//while (!inited()) {
	//	Sleep(waitInterval);
	//}
	
	//postInit();

	if (!APGlobalConfig::getInstance()->isManualPosition()) {
		APAccountInfo::getInstance()->verify();
	}

	if (APGlobalConfig::getInstance()->useInput()) {
		APInputSystem::getInstance()->init();
	}

	setInited();
}

void APFuturesFramework::onStartTransactionDay()
{
	APAccountInfo::getInstance()->onNewTransactionDay();
	APAccountInfo::getInstance()->queryAllPosition();

	APPositionManager::getInstance()->onStartTransactionDay();

	setPause(false);
}

void APFuturesFramework::onEndTransactionDay()
{
	APPositionManager::getInstance()->onEndTransactionDay();

	APTrade* trader = APTradeManager::getInstance()->getTradeInstance();
	trader->removeLocalOrders();

	setPause(true);
}

#ifdef USE_CTP
void APFuturesFramework::onCTPApiInited(BYTE flag)
{
	m_initFlag |= flag;
}

bool APFuturesFramework::isCTPInited()
{
	if (m_initFlag == CTP_Flag_All) {
		return true;
	}

	return false;
}

void APFuturesFramework::initCTP()
{
	APFuturesCTPMDAgent::getInstance()->registerInitCallback(onCTPMdInited);
	APFuturesCTPMDAgent::getInstance()->init();

	APFuturesCTPTraderAgent::getInstance()->registerInitCallback(onCTPTradeInited);
	APFuturesCTPTraderAgent::getInstance()->init();
}

#endif




