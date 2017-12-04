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

void APSimFuturesTrade::open(APORDERID orderID, APASSETID commodityID, APTrendType trend, double price, long volume, APTradeOrderType ot)
{
	APTradeOrderInfo order = {orderID, TDT_Open, commodityID, price, volume, trend};

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

void APSimFuturesTrade::close(APORDERID orderID, APASSETID commodityID, APTrendType trend, double price, long volume, APTradeOrderType ot)
{
	APTradeOrderInfo order = { orderID, TDT_Close, commodityID, price, volume, trend };

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
	APSimTradeSystem::getInstance()->requestCancel(orderID);
}

void APSimFuturesTrade::onFundChanged(APASSETID commodityID, APTradeType type, double variableFund, APORDERID orderID, APTrendType trend)
{
	APSimAccount::getInstance()->saveFund(variableFund);
}

//void APSimFuturesTrade::cancel(APASSETID commodityID, APTradeType type, APTrendType trend, double price, long volume)
//{
//	//int orderID = findOrder(commodityID, type, price, volume, trend);
//	//if (orderID >= 0) {
//	//	APSimTradeSystem::getInstance()->requestCancel(orderID);
//	//}
//}

//void APSimFuturesTrade::onTradeFinished(APASSETID commodityID, APTradeType type, double price, long volume, APORDERID orderID, APTrendType trend)
//{
//	//APORDERID orderID = UNDISTURBED_ORDER_ID;
//	//if (type == TDT_Open || type == TDT_Close) {
//	//	orderID = findOrder(commodityID, type, price, trend);
//	//	if (orderID >= 0) {
//	//		m_orderRecord[orderID].volume -= volume;
//	//		if (m_orderRecord[orderID].volume <= 0) {
//	//			m_orderRecord.erase(orderID);
//	//		}
//	//	}
//	//}
//	//else {
//	//	orderID = findOrder(commodityID, type, price, volume, trend);
//	//	if (orderID >= 0) {
//	//		m_orderRecord.erase(orderID);
//	//	}
//	//}
//	APTrade::onTradeFinished(commodityID, type, price, volume, orderID, trend);
//}
//
//int APSimFuturesTrade::findOrder(APASSETID commodityID, APTradeType type, double price, APTrendType trend)
//{
//	std::map<int, APTradeOrderInfo>::iterator it;
//	for (it = m_orderRecord.begin(); it != m_orderRecord.end(); it++) {
//		const APTradeOrderInfo& order = it->second;
//		if (commodityID == order.commodityID && type == order.type && trend == order.trend
//			&& fabs(price - order.price) < DBL_EPSILON) {
//			return it->first;
//		}
//	}
//
//	return -1;
//}
//
//int APSimFuturesTrade::findOrder(APASSETID commodityID, APTradeType type, double price, long volume, APTrendType trend)
//{
//	std::map<int, APTradeOrderInfo>::iterator it;
//	for (it = m_orderRecord.begin(); it != m_orderRecord.end(); it++) {
//		const APTradeOrderInfo& order = it->second;
//		if (commodityID == order.commodityID && type == order.type && trend == order.trend
//			&& fabs(price - order.price) < DBL_EPSILON && volume == order.volume) {
//			return it->first;
//		}
//	}
//
//	return -1;
//}
