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

void APSimFuturesTrade::open(APORDERID orderID, APASSETID instrumentID, APTradeDirection direction, double price, long volume, APOrderTimeCondition ot)
{
	APSYSTEMID sysID = APSimTradeSystem::getInstance()->requestOpen(orderID, instrumentID, direction, price, volume);
	APTradeOrderInfo order = {orderID, TT_Open, instrumentID, price, volume, direction, OS_Touched, sysID, 0};

	m_orderRecord[orderID] = order;

	std::string directionStr;
	if (direction == TD_Buy) {
		directionStr = "Buy";
	}
	else if (direction == TD_Sell) {
		directionStr = "Sell";
	}
	APLogger->log(">>>> Open %s position price: %f, volume: %d. ", directionStr.c_str(), price, volume);
}

void APSimFuturesTrade::close(APORDERID orderID, APASSETID instrumentID, APTradeDirection direction, double price, long volume, APOrderTimeCondition ot)
{
	APSYSTEMID sysID = APSimTradeSystem::getInstance()->requestClose(orderID, instrumentID, direction, price, volume);
	APTradeOrderInfo order = { orderID, TT_Close, instrumentID, price, volume, direction, OS_Touched, sysID, 0 };

	m_orderRecord[orderID] = order;

	std::string directionStr;
	if (direction == TD_Buy) {
		directionStr = "Buy";
	}
	else if (direction == TD_Sell) {
		directionStr = "Sell";
	}
	APLogger->log("<<<< Close %s position price: %f, volume: %d. ", directionStr.c_str(), price, volume);
}

void APSimFuturesTrade::cancel(APORDERID orderID)
{
	APSYSTEMID sysID = getSysIDByOrder(orderID);
	if (sysID.length() > 0) {
		APSimTradeSystem::getInstance()->requestCancel(sysID);
	}
}

void APSimFuturesTrade::onFundChanged(APASSETID instrumentID, APTradeType type, double variableFund, APORDERID orderID, APTradeDirection direction)
{
	APSimAccount::getInstance()->saveFund(variableFund);
}

//void APSimFuturesTrade::cancel(APASSETID instrumentID, APTradeType type, APTradeDirection direction, double price, long volume)
//{
//	//int orderID = findOrder(instrumentID, type, price, volume, direction);
//	//if (orderID >= 0) {
//	//	APSimTradeSystem::getInstance()->requestCancel(orderID);
//	//}
//}

//void APSimFuturesTrade::onTradeDealt(APASSETID instrumentID, APTradeType type, double price, long volume, APORDERID orderID, APTradeDirection direction)
//{
//	//APORDERID orderID = INVALID_ORDER_ID;
//	//if (type == TT_Open || type == TT_Close) {
//	//	orderID = findOrder(instrumentID, type, price, direction);
//	//	if (orderID >= 0) {
//	//		m_orderRecord[orderID].volume -= volume;
//	//		if (m_orderRecord[orderID].volume <= 0) {
//	//			m_orderRecord.erase(orderID);
//	//		}
//	//	}
//	//}
//	//else {
//	//	orderID = findOrder(instrumentID, type, price, volume, direction);
//	//	if (orderID >= 0) {
//	//		m_orderRecord.erase(orderID);
//	//	}
//	//}
//	APTrade::onTradeDealt(instrumentID, type, price, volume, orderID, direction);
//}
//
//int APSimFuturesTrade::findOrder(APASSETID instrumentID, APTradeType type, double price, APTradeDirection direction)
//{
//	std::map<int, APTradeOrderInfo>::iterator it;
//	for (it = m_orderRecord.begin(); it != m_orderRecord.end(); it++) {
//		const APTradeOrderInfo& order = it->second;
//		if (instrumentID == order.instrumentID && type == order.type && direction == order.direction
//			&& fabs(price - order.price) < DBL_EPSILON) {
//			return it->first;
//		}
//	}
//
//	return -1;
//}
//
//int APSimFuturesTrade::findOrder(APASSETID instrumentID, APTradeType type, double price, long volume, APTradeDirection direction)
//{
//	std::map<int, APTradeOrderInfo>::iterator it;
//	for (it = m_orderRecord.begin(); it != m_orderRecord.end(); it++) {
//		const APTradeOrderInfo& order = it->second;
//		if (instrumentID == order.instrumentID && type == order.type && direction == order.direction
//			&& fabs(price - order.price) < DBL_EPSILON && volume == order.volume) {
//			return it->first;
//		}
//	}
//
//	return -1;
//}
