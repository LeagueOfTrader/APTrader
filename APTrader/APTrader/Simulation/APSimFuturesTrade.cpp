#include "APSimFuturesTrade.h"
#include "APSimTradeSystem.h"
#include "../Utils/APLog.h"
#include "APSimAccount.h"
#include "../APPositionCtrl.h"
#include <vector>
#include "../APTypes.h"

APSimFuturesTrade::APSimFuturesTrade()
{
	m_orderRecord.clear();
}


APSimFuturesTrade::~APSimFuturesTrade()
{
	m_orderRecord.clear();
}

void APSimFuturesTrade::open(APORDERID orderID, APASSETID instrumentID, APTrendType trend, double price, long volume, APOrderTimeCondition ot)
{
	APSYSTEMID sysID = APSimTradeSystem::getInstance()->requestOpen(orderID, instrumentID, trend, price, volume);
	APTradeOrderInfo order = {orderID, TDT_Open, instrumentID, price, volume, trend, TS_Ordered, sysID, 0};

	m_orderRecord[orderID] = order;

	std::string trendStr;
	if (trend == TT_Long) {
		trendStr = "Long";
	}
	else if (trend == TT_Short) {
		trendStr = "Short";
	}
	APLogger->log(">>>> Open %s position price: %f, volume: %d. ", trendStr.c_str(), price, volume);
}

void APSimFuturesTrade::close(APORDERID orderID, APASSETID instrumentID, APTrendType trend, double price, long volume, APOrderTimeCondition ot)
{
	APSYSTEMID sysID = APSimTradeSystem::getInstance()->requestClose(orderID, instrumentID, trend, price, volume);
	APTradeOrderInfo order = { orderID, TDT_Close, instrumentID, price, volume, trend, TS_Ordered, sysID, 0 };

	m_orderRecord[orderID] = order;

	std::string trendStr;
	if (trend == TT_Long) {
		trendStr = "Long";
	}
	else if (trend == TT_Short) {
		trendStr = "Short";
	}
	APLogger->log("<<<< Close %s position price: %f, volume: %d. ", trendStr.c_str(), price, volume);
}

void APSimFuturesTrade::cancel(APORDERID orderID)
{
	APSYSTEMID sysID = getSysIDByOrder(orderID);
	if (sysID > 0) {
		APSimTradeSystem::getInstance()->requestCancel(sysID);
	}
}

void APSimFuturesTrade::onFundChanged(APASSETID instrumentID, APTradeType type, double variableFund, APORDERID orderID, APTrendType trend)
{
	APSimAccount::getInstance()->saveFund(variableFund);
}

//void APSimFuturesTrade::cancel(APASSETID instrumentID, APTradeType type, APTrendType trend, double price, long volume)
//{
//	//int orderID = findOrder(instrumentID, type, price, volume, trend);
//	//if (orderID >= 0) {
//	//	APSimTradeSystem::getInstance()->requestCancel(orderID);
//	//}
//}

//void APSimFuturesTrade::onTradeDealt(APASSETID instrumentID, APTradeType type, double price, long volume, APORDERID orderID, APTrendType trend)
//{
//	//APORDERID orderID = UNDISTURBED_ORDER_ID;
//	//if (type == TDT_Open || type == TDT_Close) {
//	//	orderID = findOrder(instrumentID, type, price, trend);
//	//	if (orderID >= 0) {
//	//		m_orderRecord[orderID].volume -= volume;
//	//		if (m_orderRecord[orderID].volume <= 0) {
//	//			m_orderRecord.erase(orderID);
//	//		}
//	//	}
//	//}
//	//else {
//	//	orderID = findOrder(instrumentID, type, price, volume, trend);
//	//	if (orderID >= 0) {
//	//		m_orderRecord.erase(orderID);
//	//	}
//	//}
//	APTrade::onTradeDealt(instrumentID, type, price, volume, orderID, trend);
//}
//
//int APSimFuturesTrade::findOrder(APASSETID instrumentID, APTradeType type, double price, APTrendType trend)
//{
//	std::map<int, APTradeOrderInfo>::iterator it;
//	for (it = m_orderRecord.begin(); it != m_orderRecord.end(); it++) {
//		const APTradeOrderInfo& order = it->second;
//		if (instrumentID == order.instrumentID && type == order.type && trend == order.trend
//			&& fabs(price - order.price) < DBL_EPSILON) {
//			return it->first;
//		}
//	}
//
//	return -1;
//}
//
//int APSimFuturesTrade::findOrder(APASSETID instrumentID, APTradeType type, double price, long volume, APTrendType trend)
//{
//	std::map<int, APTradeOrderInfo>::iterator it;
//	for (it = m_orderRecord.begin(); it != m_orderRecord.end(); it++) {
//		const APTradeOrderInfo& order = it->second;
//		if (instrumentID == order.instrumentID && type == order.type && trend == order.trend
//			&& fabs(price - order.price) < DBL_EPSILON && volume == order.volume) {
//			return it->first;
//		}
//	}
//
//	return -1;
//}
