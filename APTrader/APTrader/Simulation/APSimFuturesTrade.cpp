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

void APSimFuturesTrade::open(APASSETID commodityID, APTrendType trend, double price, long amount, APTradeOrderType ot)
{
	UINT orderID = APSimTradeSystem::getInstance()->requestOpen(commodityID, trend, price, amount);
	APTradeOrderInfo order = {orderID, TDT_Open, commodityID, price, amount, trend};

	m_orderRecord[orderID] = order;

	std::string trendStr;
	if (trend == TT_Long) {
		trendStr = "Long";
	}
	else if (trend == TT_Short) {
		trendStr = "Short";
	}
	APLogger->log(">>>> Open %s position price: %f, amount: %d. ", trendStr.c_str(), price, amount);
}

void APSimFuturesTrade::close(APASSETID commodityID, APTrendType trend, double price, long amount, APTradeOrderType ot)
{
	UINT orderID = APSimTradeSystem::getInstance()->requestClose(commodityID, trend, price, amount);
	APTradeOrderInfo order = { orderID, TDT_Close, commodityID, price, amount, trend };

	m_orderRecord[orderID] = order;

	std::string trendStr;
	if (trend == TT_Long) {
		trendStr = "Long";
	}
	else if (trend == TT_Short) {
		trendStr = "Short";
	}
	APLogger->log("<<<< Close %s position price: %f, amount: %d. ", trendStr.c_str(), price, amount);
}

void APSimFuturesTrade::onFundChanged(APASSETID commodityID, APTradeType type, double variableFund, APORDERID orderID, APTrendType trend)
{
	APSimAccount::getInstance()->saveFund(variableFund);
}

//void APSimFuturesTrade::cancel(APASSETID commodityID, APTradeType type, APTrendType trend, double price, long amount)
//{
//	//int orderID = findOrder(commodityID, type, price, amount, trend);
//	//if (orderID >= 0) {
//	//	APSimTradeSystem::getInstance()->requestCancel(orderID);
//	//}
//}

//void APSimFuturesTrade::onTradeFinished(APASSETID commodityID, APTradeType type, double price, long amount, APORDERID orderID, APTrendType trend)
//{
//	//APORDERID orderID = UNDISTURBED_ORDER_ID;
//	//if (type == TDT_Open || type == TDT_Close) {
//	//	orderID = findOrder(commodityID, type, price, trend);
//	//	if (orderID >= 0) {
//	//		m_orderRecord[orderID].amount -= amount;
//	//		if (m_orderRecord[orderID].amount <= 0) {
//	//			m_orderRecord.erase(orderID);
//	//		}
//	//	}
//	//}
//	//else {
//	//	orderID = findOrder(commodityID, type, price, amount, trend);
//	//	if (orderID >= 0) {
//	//		m_orderRecord.erase(orderID);
//	//	}
//	//}
//	APTrade::onTradeFinished(commodityID, type, price, amount, orderID, trend);
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
//int APSimFuturesTrade::findOrder(APASSETID commodityID, APTradeType type, double price, long amount, APTrendType trend)
//{
//	std::map<int, APTradeOrderInfo>::iterator it;
//	for (it = m_orderRecord.begin(); it != m_orderRecord.end(); it++) {
//		const APTradeOrderInfo& order = it->second;
//		if (commodityID == order.commodityID && type == order.type && trend == order.trend
//			&& fabs(price - order.price) < DBL_EPSILON && amount == order.amount) {
//			return it->first;
//		}
//	}
//
//	return -1;
//}
