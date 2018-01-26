#include "APFuturesFramework.h"
#include "../Utils/APRedisAgent.h"
#include "../APAccountInfo.h"
#include "../APTradeManager.h"
#include "../APStrategyManager.h"
#include "../APMarketDataManager.h"
#include "../APGlobalConfig.h"
#include "../APSystemSetting.h"
#include <windows.h>

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

APFuturesFramework::APFuturesFramework()
{	
	m_finished = false;

#ifdef USE_CTP
	m_initFlag = CTP_Flag_None;
#endif
}

APFuturesFramework::~APFuturesFramework()
{
}

void APFuturesFramework::init() 
{
	APGlobalConfig::getInstance()->init();
	APRedisAgent::getInstance()->init();
	APSystemSetting::getInstance()->init();

#ifdef USE_CTP
	initCTP();
#else
	initLocalSystem();
#endif // USE_CTP

}

void APFuturesFramework::update(float deltaTime)
{
	APMarketDataMgr->update(deltaTime);
	APAccountInfo::getInstance()->update();
	APStrategyManager::getInstance()->update();
}

void APFuturesFramework::exit()
{
	APStrategyManager::getInstance()->exit();
	APTradeManager::getInstance()->exit();
}

bool APFuturesFramework::finished()
{
	return m_finished;
}

void APFuturesFramework::initLocalSystem()
{
	APMarketDataMgr->init();
	
	APAccountInfo::getInstance()->init();
	APTradeManager::getInstance()->init();
	//
	APTrade* trader = APTradeManager::getInstance()->getTradeInstance();
	if (trader == NULL) {
		return;
	}

	while (!trader->inited() || !APAccountInfo::getInstance()->inited()) {
		Sleep(10);
	}
	APStrategyManager::getInstance()->init();
	APAccountInfo::getInstance()->verify();
}

#ifdef USE_CTP
void APFuturesFramework::onCTPApiInited(BYTE flag)
{
	m_initFlag |= flag;

	if (m_initFlag == CTP_Flag_All) {
		initLocalSystem();
	}
}

void APFuturesFramework::initCTP()
{
	APFuturesCTPMDAgent::getInstance()->registerInitCallback(onCTPMdInited);
	APFuturesCTPMDAgent::getInstance()->init();

	APFuturesCTPTraderAgent::getInstance()->registerInitCallback(onCTPTradeInited);
	APFuturesCTPTraderAgent::getInstance()->init();
}

#endif




