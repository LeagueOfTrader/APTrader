#include "APSharesPositionCtrl.h"
#include "../Trade/APSharesTrade.h"
#include "../APTradeManager.h"

APSharesPositionCtrl::APSharesPositionCtrl()
{
	m_trade = NULL;
}


APSharesPositionCtrl::~APSharesPositionCtrl()
{
	if (m_trade != NULL) {
		//APTradeManager::getInstance()->unregisterPositionCtrl(this);
		m_trade = NULL;
	}
}

void APSharesPositionCtrl::init(std::string positionInfo)
{
	setBaseParam(positionInfo);
	//APTradeManager::getInstance()->registerPositionCtrl(m_commodityID, TT_Long, this);
	m_trade = dynamic_cast<APSharesTrade*>(APTradeManager::getInstance()->getTradeInstance());
}

void APSharesPositionCtrl::open(APTrendType type, double price, long amount)
{
	m_trade->open(m_commodityID, type, price, amount, this);
}

void APSharesPositionCtrl::close(APTrendType type, double price, long amount)
{
	m_trade->close(m_commodityID, type, price, amount, this);
}

void APSharesPositionCtrl::openAll(APTrendType type, double price)
{
	//
}

void APSharesPositionCtrl::closeAll(APTrendType type, double price)
{
	//
}

void APSharesPositionCtrl::cancel(APTradeType type, double price, long amount)
{
}

void APSharesPositionCtrl::cancel(APTradeType type)
{
}

void APSharesPositionCtrl::cancelAll()
{
}

void APSharesPositionCtrl::onTradeFinished(APASSETID commodityID, APTradeType type,  double price, long amount, APTrendType trend)
{
	switch (type) {
		case TDT_Open:
			m_frozenPosition += amount;
			m_openOrdersPosition -= amount;
			break;
		case TDT_Close:
			m_availablePosition += amount;
			m_closeOrdersPosition -= amount;
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
