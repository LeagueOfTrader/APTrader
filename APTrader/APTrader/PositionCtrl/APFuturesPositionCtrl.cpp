#include "APFuturesPositionCtrl.h"
#include "../APTradeManager.h"

APFuturesPositionCtrl::APFuturesPositionCtrl()
{
	m_trade = NULL;
}


APFuturesPositionCtrl::~APFuturesPositionCtrl()
{
	if (m_trade != NULL) {
		//APTradeManager::getInstance()->unregisterPositionCtrl(this);
		m_trade = NULL;
	}	
}

void APFuturesPositionCtrl::init(std::string positionInfo)
{
	setBaseParam(positionInfo);
	//APTradeManager::getInstance()->registerPositionCtrl(m_commodityID, m_trendType, this);
	m_trade = dynamic_cast<APFuturesTrade*>(APTradeManager::getInstance()->getTradeInstance());
}


void APFuturesPositionCtrl::open(APTrendType type, double price, long amount)
{
	open(m_commodityID, type, price, amount);
}

void APFuturesPositionCtrl::close(APTrendType type, double price, long amount)
{
	close(m_commodityID, type, price, amount);
}

void APFuturesPositionCtrl::openAll(APTrendType type, double price)
{
	open(type, price, m_availablePosition);
}

void APFuturesPositionCtrl::closeAll(APTrendType type, double price)
{
	close(type, price, m_holdPosition);
}

void APFuturesPositionCtrl::cancel(APTradeType type, double price, long amount)
{
}

void APFuturesPositionCtrl::cancel(APTradeType type, double price, APTrendType trend)
{
	if (m_trade != NULL) {
		m_trade->cancel(m_commodityID, type, trend, price, this);
	}
}

void APFuturesPositionCtrl::cancel(APTradeType type)
{
	if (m_trade != NULL) {
		m_trade->cancel(m_commodityID, type, this);
	}
}

void APFuturesPositionCtrl::cancelAll()
{
	if (m_trade != NULL) {
		m_trade->cancelAll(m_commodityID, this);
	}
}

void APFuturesPositionCtrl::onTradeFinished(APASSETID commodityID, APTradeType type,  double price, long amount, APTrendType trend)
{
	if (commodityID != m_commodityID || trend != m_trendType) {
		return;
	}

	switch (type) {
	case TDT_Open:
		m_holdPosition += amount;
		m_openOrdersPosition -= amount;
		//changeContractPossession(commodityID, amount, trend);
		break;
	case TDT_Close:
		m_availablePosition += amount;
		m_closeOrdersPosition -= amount;
		//changeContractPossession(commodityID, -amount, trend);
		break;
	case TDT_CancelOpen:
		m_availablePosition += amount;
		m_openOrdersPosition -= amount;
		break;
	case TDT_CancelClose:
		m_holdPosition += amount;
		m_closeOrdersPosition -= amount;
		break;
	default:
		break;
	}
}

void APFuturesPositionCtrl::setContractID(APASSETID contractID)
{
	m_commodityID = contractID;
}

void APFuturesPositionCtrl::setContractType(APTrendType type)
{
	m_trendType = type;
}

void APFuturesPositionCtrl::open(APASSETID contractID, APTrendType trend, double price, long amount, APTradeOrderType ot)
{
	if (m_trade != NULL) {
		m_trade->open(contractID, trend, price, amount, this, ot);
	}
}

void APFuturesPositionCtrl::close(APASSETID contractID, APTrendType trend, double price, long amount, APTradeOrderType ot)
{
	if (m_trade != NULL) {
		m_trade->close(contractID, trend, price, amount, this, ot);
	}
}





