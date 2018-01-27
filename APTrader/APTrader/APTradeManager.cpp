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
	
	if (m_trader != NULL) {
		//m_trader->load();
		m_trader->queryAllOrders();
		setInited();
	}
}

void APTradeManager::exit()
{
	if (m_trader != NULL)
	{
		//m_trader->save();
		m_trader->exit();
		delete m_trader;
		m_trader = NULL;
	}

	m_exited = true;
}

bool APTradeManager::inited()
{
	if (m_trader != NULL) {
		return m_trader->inited();
	}

	return false;
}

void APTradeManager::setTradeInstance(APTrade * tradeInstance)
{
	m_trader = tradeInstance;
}

APTrade * APTradeManager::getTradeInstance()
{
	return m_trader;
}