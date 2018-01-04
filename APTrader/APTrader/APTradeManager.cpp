#include "APTradeManager.h"
#include "APGlobalConfig.h"
#include "APObjectFactory.h"
#include "APTrade.h"

APTradeManager::APTradeManager()
{
	//
}


APTradeManager::~APTradeManager()
{
	if (m_trader != NULL) {
		delete m_trader;
	}
}

//void APTradeManager::registerPositionCtrl(APASSETID instrumentID, APTradeDirection direction, APPositionCtrl * positionCtrl)
//{
//	if (m_trader != NULL) {
//		m_trader->registerPositionCtrl(instrumentID, direction, positionCtrl);
//	}
//}
//
//void APTradeManager::unregisterPositionCtrl(APPositionCtrl * positionCtrl)
//{
//	if (m_trader != NULL) {
//		m_trader->unregisterPositionCtrl(positionCtrl);
//	}
//}

void APTradeManager::init()
{
	m_trader = APObjectFactory::newTrade(APGlobalConfig::getInstance()->getInstrumentType());
}

void APTradeManager::setTradeInstance(APTrade * tradeInstance)
{
	m_trader = tradeInstance;
}

APTrade * APTradeManager::getTradeInstance()
{
	return m_trader;
}
