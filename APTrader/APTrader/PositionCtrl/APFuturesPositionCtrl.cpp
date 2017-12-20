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
	//APTradeManager::getInstance()->registerPositionCtrl(m_instrumentID, m_trendType, this);
	m_trade = dynamic_cast<APFuturesTrade*>(APTradeManager::getInstance()->getTradeInstance());
}


void APFuturesPositionCtrl::open(APTrendType type, double price, long volume)
{
	open(m_instrumentID, type, price, volume);
}

void APFuturesPositionCtrl::close(APTrendType type, double price, long volume)
{
	close(m_instrumentID, type, price, volume);
}

void APFuturesPositionCtrl::openAll(APTrendType type, double price)
{
	open(type, price, m_availablePosition);
}

void APFuturesPositionCtrl::closeAll(APTrendType type, double price)
{
	close(type, price, m_holdPosition);
}

//void APFuturesPositionCtrl::cancel(APTradeType type, double price, long volume)
//{
//}

void APFuturesPositionCtrl::cancel(APTradeType type, double price, APTrendType trend)
{
	if (m_trade != NULL) {
		//m_trade->cancel(m_instrumentID, type, trend, price, this);
		std::list<APORDERID>::iterator it;
		APTradeOrderInfo info;
		if (type == TDT_Open) {
			for (it = m_openOrderList.begin(); it != m_openOrderList.end(); ) {			
				if (m_trade->getOrderInfo(*it, info)) {
					if (info.trend == trend) {
						if (fabs(info.price - price) < DBL_EPSILON ||
							(trend == TT_Short && info.price > price) ||  //Short Open, cancel higher offered price
							(trend == TT_Long &&  info.price < price)) { // Long Open, cancel lower offered price
							APORDERID orderID = *it;
							std::list<APORDERID>::iterator itNext = ++it;
							m_trade->cancel(orderID, this);
							it = itNext;
							continue;
						}
					}
				}
				it++;
			}
		}
		else if (type == TDT_Close) {
			for (it = m_closeOrderList.begin(); it != m_closeOrderList.end();) {
				if (m_trade->getOrderInfo(*it, info)) {
					if (info.trend == trend) {
						if (fabs(info.price - price) < DBL_EPSILON ||
							(trend == TT_Long  && info.price > price) ||  // Long Close, cancel higher offered price
							(trend == TT_Short && info.price < price)) { // Short Close, cancel lower offered price
							APORDERID orderID = *it;
							std::list<APORDERID>::iterator itNext = ++it;
							m_trade->cancel(orderID, this);
							it = itNext;
							continue;
						}
					}
				}
				it++;
			}
		}
	}
}

void APFuturesPositionCtrl::cancel(APTradeType type)
{
	if (m_trade != NULL) {
		//m_trade->cancel(m_instrumentID, type, this);
		std::list<APORDERID>::iterator it;
		if (type == TDT_Open) {
			for (it = m_openOrderList.begin(); it != m_openOrderList.end(); it++) {
				m_trade->cancel(*it, this);
			}
		}
		else if (type == TDT_Close) {
			for (it = m_closeOrderList.begin(); it != m_closeOrderList.end(); it++) {
				m_trade->cancel(*it, this);
			}
		}
	}
}

//void APFuturesPositionCtrl::cancelAll()
//{
//	if (m_trade != NULL) {
//		std::list<APORDERID>::iterator it;
//		for (it = m_openOrderList.begin(); it != m_openOrderList.end(); it++) {
//			m_trade->cancel(*it, this);
//		}
//		for (it = m_closeOrderList.begin(); it != m_closeOrderList.end(); it++) {
//			m_trade->cancel(*it, this);
//		}
//	}
//}

void APFuturesPositionCtrl::onTradeDealt(APASSETID instrumentID, APTradeType type,  double price, long deltaVolume, APORDERID orderID, APTrendType trend)
{
	//if (instrumentID != m_instrumentID || trend != m_trendType) {
	//	return;
	//}

	switch (type) {
	case TDT_Open:
		m_holdPosition += deltaVolume;
		m_openOrdersPosition -= deltaVolume;
		//changeContractPossession(instrumentID, volume, trend);
		break;
	case TDT_Close:
		m_availablePosition += deltaVolume;
		m_closeOrdersPosition -= deltaVolume;
		//changeContractPossession(instrumentID, -volume, trend);
		break;
	default:
		break;
	}
}

void APFuturesPositionCtrl::onTradeCanceled(APASSETID instrumentID, APTradeType type, long volume, APORDERID orderID, APTrendType trend)
{
	switch (type) {
	case TDT_Open:
		m_availablePosition += volume;
		m_openOrdersPosition -= volume;
		m_openOrderList.remove(orderID);
		break;
	case TDT_Close:
		m_holdPosition += volume;
		m_closeOrdersPosition -= volume;
		m_closeOrderList.remove(orderID);
		break;
	default:
		break;
	}
}

void APFuturesPositionCtrl::setContractID(APASSETID contractID)
{
	m_instrumentID = contractID;
}

void APFuturesPositionCtrl::setContractType(APTrendType type)
{
	m_trendType = type;
}

void APFuturesPositionCtrl::open(APASSETID instrumentID, APTrendType trend, 
									APOrderPriceType orderPriceType, double price, 
									APOrderTimeCondition orderTimeCondition, std::string date, 
									APOrderVolumeCondition orderVolumeCondition, long volume, long minVolume, 
									APOrderContingentCondition orderContingentCondition, double stopPrice)
{
	if (m_trade != NULL) {
		APORDERID orderID = m_trade->open(instrumentID, trend,
			orderPriceType, price, this,
			orderTimeCondition, date,
			orderVolumeCondition, volume, minVolume,
			orderContingentCondition, stopPrice);
		m_openOrderList.push_back(orderID);
	}
}

void APFuturesPositionCtrl::close(APASSETID instrumentID, APTrendType trend, 
									APOrderPriceType orderPriceType, double price, 
									APOrderTimeCondition orderTimeCondition, std::string date, 
									APOrderVolumeCondition orderVolumeCondition, long volume, long minVolume, 
									APOrderContingentCondition orderContingentCondition, double stopPrice)
{
	if (m_trade != NULL) {
		APORDERID orderID = m_trade->close(instrumentID, trend,
			orderPriceType, price, this,
			orderTimeCondition, date,
			orderVolumeCondition, volume, minVolume,
			orderContingentCondition, stopPrice);

		m_closeOrderList.push_back(orderID);

	}
}

void APFuturesPositionCtrl::open(APASSETID instrumentID, APTrendType trend, double price, long volume, APOrderTimeCondition ot)
{
	if (m_trade != NULL) {
		APORDERID orderID = m_trade->open(instrumentID, trend, price, volume, this, ot);
		if (ot == OTC_GoodForDay) {
			m_openOrderList.push_back(orderID);
		}
	}
}

void APFuturesPositionCtrl::close(APASSETID instrumentID, APTrendType trend, double price, long volume, APOrderTimeCondition ot)
{
	if (m_trade != NULL) {
		APORDERID orderID = m_trade->close(instrumentID, trend, price, volume, this, ot);
		if (ot == OTC_GoodForDay) {
			m_closeOrderList.push_back(orderID);
		}
	}
}





