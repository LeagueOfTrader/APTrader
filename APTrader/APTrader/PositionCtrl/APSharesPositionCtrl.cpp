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
	initWithData(positionInfo);
	//APTradeManager::getInstance()->registerPositionCtrl(m_instrumentID, TD_Buy, this);
	m_trade = dynamic_cast<APSharesTrade*>(APTradeManager::getInstance()->getTradeInstance());
}

void APSharesPositionCtrl::open(APTradeDirection direction, double price, long volume)
{
	APORDERID orderID = m_trade->open(m_instrumentID, direction, price, volume, this);
	if (orderID != INVALID_ORDER_ID) {
		m_openOrderList.push_back(orderID);
	}
}

void APSharesPositionCtrl::close(APTradeDirection direction, double price, long volume)
{
	APORDERID orderID = m_trade->close(m_instrumentID, direction, price, volume, this);
	if (orderID != INVALID_ORDER_ID) {
		m_closeOrderList.push_back(orderID);
	}
}

void APSharesPositionCtrl::openAll(APTradeDirection direction, double price)
{
	//
}

void APSharesPositionCtrl::closeAll(APTradeDirection direction, double price)
{
	//
}

void APSharesPositionCtrl::cancel(APTradeType type, double price, APTradeDirection direction)
{
	if (direction != TD_Buy) {
		return;
	}

	if (m_trade != NULL) {
		//m_trade->cancel(m_instrumentID, type, direction, price, this);
		std::list<APORDERID>::iterator it;
		APTradeOrderInfo info;
		if (type == TT_Open) {
			for (it = m_openOrderList.begin(); it != m_openOrderList.end(); it++) {
				//m_trade->cancel(*it, this);				
				if (m_trade->getOrderInfo(*it, info)) {					
					if (fabs(info.price - price) < DBL_EPSILON ||						
						info.price < price) { 
						m_trade->cancel(*it, this);
					}					
				}
			}
		}
		else if (type == TT_Close) {
			for (it = m_closeOrderList.begin(); it != m_closeOrderList.end(); it++) {
				if (m_trade->getOrderInfo(*it, info)) {
					if (fabs(info.price - price) < DBL_EPSILON ||
						info.price > price) {
						m_trade->cancel(*it, this);
					}					
				}
			}
		}
	}
}

void APSharesPositionCtrl::cancel(APTradeType type)
{
}

//void APSharesPositionCtrl::cancelAll()
//{
//}

void APSharesPositionCtrl::onTradeDealt(APASSETID instrumentID, APTradeType type,  double price, long deltaVolume, APORDERID orderID, APTradeDirection direction)
{
	switch (type) {
		case TT_Open:
			m_frozenPosition += deltaVolume;
			m_openOrdersPosition -= deltaVolume;
			break;
		case TT_Close:
			m_marginPosition += deltaVolume;
			m_closeOrdersPosition -= deltaVolume;
			break;

		default:
			break;
	}
}

void APSharesPositionCtrl::onTradeCanceled(APASSETID instrumentID, APTradeType type, long volume, APORDERID orderID, APTradeDirection direction)
{
	switch (type) {
	case TT_Open:
		m_marginPosition += volume;
		m_openOrdersPosition -= volume;
		m_openOrderList.remove(orderID);
		break;
	case TT_Close:
		m_availablePosition += volume;
		m_closeOrdersPosition -= volume;
		m_closeOrderList.remove(orderID);
		break;
	default:
		break;
	}
}
