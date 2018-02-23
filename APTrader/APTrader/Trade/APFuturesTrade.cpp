#include "APFuturesTrade.h"
#include "../APMacro.h"
#include "../APAccountInfo.h"
#include "../Futures/APFuturesIDSelector.h"

#ifdef USE_CTP
#include "../Impl/CTP/APFuturesCTPTraderAgent.h"
#endif

std::set<std::string> APFuturesTrade::m_closeTodayFirstList;

APFuturesTrade::APFuturesTrade()
{
}


APFuturesTrade::~APFuturesTrade()
{
}

void APFuturesTrade::open(APORDERID orderID, APASSETID instrumentID, APTradeDirection direction, double price, long volume, APOrderTimeCondition ot)
{
#ifdef USE_CTP
	APFuturesCTPTraderAgent::getInstance()->applyOrder(TT_Open, instrumentID, price, volume, orderID, direction);
#endif
}

APORDERID APFuturesTrade::close(APASSETID instrumentID, APTradeDirection direction, double price, long volume, APPositionCtrl* pc, APOrderTimeCondition ot) {
	
	APORDERID orderID = initializeLocalOrder(instrumentID, TT_Close, direction, price, volume, pc);

	if (orderID == INVALID_ORDER_ID) {
		return orderID;
	}

	if ("SHFE" != APFuturesIDSelector::getDomesticExchangeID(instrumentID)) {
		close(orderID, instrumentID, direction, price, volume, ot);
		return orderID;
	}

	long vol = close(orderID, instrumentID, direction, price, volume, ot);
	return orderID;
}

long APFuturesTrade::close(APORDERID orderID, APASSETID instrumentID, APTradeDirection direction, double price, long volume, APOrderTimeCondition ot)
{
	long amount = 0;
	APTradeType type = TT_Close;
	APPositionData posData;
	bool ret = APAccountInfo::getInstance()->getPositionData(instrumentID, posData);
	if (ret) {
		if (isInstrumentCloseTodayFirst(instrumentID)) {
			if (posData.todayPosition > 0) {
				amount = std::min(posData.todayPosition, volume);
				posData.todayPosition -= amount;
				type = TT_CloseToday;
			}

			if (amount == 0) {
				amount = volume;
			}
		}
		else {
			if (posData.yesterdayPosition > 0) {
				amount = std::min(posData.yesterdayPosition, volume);
				posData.yesterdayPosition -= amount;
			}

			if (amount == 0) {
				amount = volume;
				type = TT_CloseToday;
			}
		}
	}

	if (type == TT_CloseToday) {
		m_closeTodayOrders.insert(orderID);
	}

#ifdef USE_CTP
	if (amount > 0) {
		APFuturesCTPTraderAgent::getInstance()->applyOrder(type, instrumentID, price, amount, orderID, direction);
	}

	return amount;
#endif
}

void APFuturesTrade::open(APASSETID instrumentID, APORDERID localOrderID, APTradeDirection direction, 
							APOrderPriceType orderPriceType, double price, 
							APOrderTimeCondition orderTimeCondition, std::string date, 
							APOrderVolumeCondition orderVolumeCondition, long volume, long minVolume, 
							APOrderContingentCondition orderContingentCondition, double stopPrice)
{
#ifdef USE_CTP
	APFuturesCTPTraderAgent::getInstance()->applyOrder(TT_Open, instrumentID, price, volume, localOrderID, direction, 
														orderPriceType, orderTimeCondition, date, orderVolumeCondition, minVolume, 
														orderContingentCondition, stopPrice);
#endif
}

void APFuturesTrade::close(APASSETID instrumentID, APORDERID localOrderID, APTradeDirection direction, 
							APOrderPriceType orderPriceType, double price, 
							APOrderTimeCondition orderTimeCondition, std::string date, 
							APOrderVolumeCondition orderVolumeCondition, long volume, long minVolume, 
							APOrderContingentCondition orderContingentCondition, double stopPrice)
{
#ifdef USE_CTP
	APFuturesCTPTraderAgent::getInstance()->applyOrder(TT_Close, instrumentID, price, volume, localOrderID, direction,
														orderPriceType, orderTimeCondition, date, orderVolumeCondition, minVolume,
														orderContingentCondition, stopPrice);
#endif
}

void APFuturesTrade::cancel(APORDERID orderID)
{
#ifdef USE_CTP
	if (!isOrderExists(orderID)) {
		return;
	}

	APSYSTEMID orderRef = getOrderRefByOrderID(orderID);
	int sessionID = getSessionIDByOrderID(orderID);
	int frontID = getFrontIDByOrderID(orderID);
	APASSETID instrumentID = getInstrumentIDByOrderID(orderID);
	APFuturesCTPTraderAgent::getInstance()->cancelOrder(instrumentID, frontID, sessionID, orderRef);	
#endif
}

void APFuturesTrade::init()
{
	APTrade::init();

	APFuturesIDSelector::init();

	m_closeTodayFirstList.clear();
	m_closeTodayFirstList.insert("rb");
	m_closeTodayFirstList.insert("hc");
	m_closeTodayFirstList.insert("i");
	m_closeTodayFirstList.insert("ag");
	m_closeTodayFirstList.insert("bu");
	m_closeTodayFirstList.insert("au");
}

bool APFuturesTrade::isInstrumentCloseTodayFirst(APASSETID instrumentID)
{
	std::string typeID = APFuturesIDSelector::getTypeID(instrumentID);
	if (m_closeTodayFirstList.find(typeID) != m_closeTodayFirstList.end()) {
		return true;
	}
	return false;
}

void APFuturesTrade::onTraded(APASSETID instrumentID, APTradeType type, double price, long volume, APORDERID orderID, APTradeDirection direction)
{
	APTrade::onTraded(instrumentID, type, price, volume, orderID, direction);

	APPositionData posData;
	bool ret = APAccountInfo::getInstance()->getPositionData(instrumentID, posData);
	if (!ret) {
		return;
	}

	bool isSHFE = (APFuturesIDSelector::getDomesticExchangeID(instrumentID) == "SHFE");
	switch (type) {
	case TT_Open:
		posData.holdPosition += volume;
		posData.todayPosition += volume;
		break;
	case TT_Close:
		posData.holdPosition -= volume;
		if (isSHFE) {
			posData.yesterdayPosition -= volume;
		}
		else {
			long vol = std::min(posData.todayPosition, volume);
			posData.todayPosition -= vol;
			if (vol < volume) {
				posData.yesterdayPosition -= (volume - vol);
			}
		}
	case TT_CloseToday:
		posData.holdPosition -= volume;
		posData.todayPosition -= volume;
		break;
	default:
		break;
	}
}

void APFuturesTrade::onCanceled(APORDERID orderID)
{
	APTrade::onCanceled(orderID);

	APTradeOrderInfo orderInfo;
	if (!getOrderInfo(orderID, orderInfo)) {
		return;
	}

	APPositionData posData;
	bool ret = APAccountInfo::getInstance()->getPositionData(orderInfo.instrumentID, posData);
	if (!ret) {
		return;
	}
	//bool isSHFE = (APFuturesIDSelector::getDomesticExchangeID(orderInfo.instrumentID) == "SHFE");

	switch (orderInfo.type) {	
	case TT_Close:
		if(m_closeTodayOrders.find(orderID) != m_closeTodayOrders.end()){
			posData.todayPosition += orderInfo.volumeSurplus;
		}
		else {
			posData.yesterdayPosition += orderInfo.volumeSurplus;
		}
		break;
	default:
		break;
	}
}
