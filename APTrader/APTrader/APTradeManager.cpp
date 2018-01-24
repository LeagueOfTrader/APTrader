#include "APTradeManager.h"
#include "APGlobalConfig.h"
#include "APObjectFactory.h"
#include "APTrade.h"
#include "APMacro.h"

#ifdef USE_CTP
#include "Impl/CTP/APFuturesCTPTraderAgent.h"
#endif // USE_CTP


APTradeManager::APTradeManager()
{
	m_inited = false;
}

APTradeManager::~APTradeManager()
{
	if (m_trader != NULL) {
		delete m_trader;
	}
}

void APTradeManager::init()
{
	m_trader = APObjectFactory::newTrade(APGlobalConfig::getInstance()->getInstrumentType());	

#ifdef USE_CTP
	int maxOrderRef = APFuturesCTPTraderAgent::getInstance()->getMaxOrderRef();
	if (m_trader != NULL) {
		m_trader->setOrderIDBase(maxOrderRef);
	}
#endif
	
	//if (m_trader != NULL) {
	//	m_trader->load();
		m_inited = true;
	//}
}

void APTradeManager::exit()
{
	if (m_trader != NULL)
	{
		m_trader->save();
	}
}

void APTradeManager::setTradeInstance(APTrade * tradeInstance)
{
	m_trader = tradeInstance;
}

APTrade * APTradeManager::getTradeInstance()
{
	return m_trader;
}

bool APTradeManager::inited()
{
	return m_inited;
}
