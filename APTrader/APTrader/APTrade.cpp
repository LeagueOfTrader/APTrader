#include "APTrade.h"
#include "APPositionCtrl.h"
#include "APPositionManager.h"

APTrade::APTrade()
{
}

APTrade::~APTrade()
{
}

void APTrade::open(APASSETID commodityID, APTrendType trend, double price, long volume, APPositionCtrl* pc, APTradeOrderType ot) {
	if (pc == NULL) {
		return;
	}

	if (ot == TOT_ValidTheDay) {
		APTradeOrderPositionInfo info = { UNDISTURBED_ORDER_ID, TDT_Open, commodityID, price, volume, trend, pc->getID()};
		m_ordersApplied.push_back(info);
	}
	else {
		APTradeOrderPositionInfo info = { MATCH_ANY_ORDER_ID, TDT_Open, commodityID, price, volume, trend, pc->getID() };
		m_quickDealOrders.push_back(info);
	}
	

	open(commodityID, trend, price, volume, ot);
}

void APTrade::close(APASSETID commodityID, APTrendType trend, double price, long volume, APPositionCtrl* pc, APTradeOrderType ot) {
	if (pc == NULL) {
		return;
	}

	if (ot == TOT_ValidTheDay) {
		APTradeOrderPositionInfo info = { UNDISTURBED_ORDER_ID, TDT_Close, commodityID, price, volume, trend, pc->getID() };
		m_ordersApplied.push_back(info);
	}
	else {
		APTradeOrderPositionInfo info = { MATCH_ANY_ORDER_ID, TDT_Close, commodityID, price, volume, trend, pc->getID() };
		m_quickDealOrders.push_back(info);
	}
	

	close(commodityID, trend, price, volume, ot);
}

void APTrade::cancel(APASSETID commodityID, APTradeType type, APTrendType trend, double price, APPositionCtrl* pc) {
	if (pc == NULL) {
		return;
	}

	std::vector<UINT> relatedOrders = getRelatedOrders(pc);

	std::vector<UINT> cancelOrders;
	for (int i = 0; i < relatedOrders.size(); i++) {
		UINT id = relatedOrders[i];
		if (m_ordersSubmitted.find(id) != m_ordersSubmitted.end()) {
			APTradeOrderInfo& info = m_ordersSubmitted[id];
			if (info.commodityID == commodityID &&
				info.type == type &&
				info.trend == trend) {
				if ( fabs(info.price - price) < DBL_EPSILON ||
					(((trend == TT_Long && type == TDT_Close) || (trend == TT_Short && type == TDT_Open)) && info.price > price) ||  // Long Close / Short Open, cancel higher offered price
					(((trend == TT_Short && type == TDT_Close) || (trend == TT_Long && type == TDT_Open)) && info.price < price) ) { // Long Open / Short Close, cancel lower offered price
					UINT orderID = info.orderID;
					cancelOrders.push_back(orderID);
				}
			}
		}
	}

	for (int i = 0; i < cancelOrders.size(); i++) {
		UINT orderID = cancelOrders[i];
		if (orderID != UNDISTURBED_ORDER_ID) {
			cancel(orderID);
		}
	}
}

void APTrade::cancel(APASSETID commodityID, APTradeType type, APPositionCtrl * pc)
{
	if (pc == NULL) {
		return;
	}

	std::vector<UINT> relatedOrders = getRelatedOrders(pc);

	std::vector<UINT> cancelOrders;
	for (int i = 0; i < relatedOrders.size(); i++) {
		UINT id = relatedOrders[i];
		if (m_ordersSubmitted.find(id) != m_ordersSubmitted.end()) {
			APTradeOrderInfo& info = m_ordersSubmitted[id];
			if (info.commodityID == commodityID &&
				info.type == type) {
				UINT orderID = info.orderID;
				cancelOrders.push_back(orderID);
			}
		}
	}

	for (int i = 0; i < cancelOrders.size(); i++) {
		UINT orderID = cancelOrders[i];
		if (orderID != UNDISTURBED_ORDER_ID) {
			cancel(orderID);
		}
	}
}

void APTrade::cancelAll(APASSETID commodityID, APPositionCtrl * pc)
{
	if (pc == NULL) {
		return;
	}

	std::vector<UINT> relatedOrders = getRelatedOrders(pc);

	std::vector<UINT> cancelOrders;
	for (int i = 0; i < relatedOrders.size(); i++) {
		UINT id = relatedOrders[i];
		if (m_ordersSubmitted.find(id) != m_ordersSubmitted.end()) {
			APTradeOrderInfo& info = m_ordersSubmitted[id];
			if (info.commodityID == commodityID) {
				UINT orderID = info.orderID;
				cancelOrders.push_back(orderID);
			}
		}
	}

	for (int i = 0; i < cancelOrders.size(); i++) {
		UINT orderID = cancelOrders[i];
		if (orderID != UNDISTURBED_ORDER_ID) {
			cancel(orderID);
		}
	}
}


void APTrade::onTradeFinished(APASSETID commodityID, APTradeType type, double price, long volume, APORDERID orderID, APTrendType trend)
{
	//if (m_positionData.find(commodityID) != m_positionData.end()) {
	//	//std::vector<APTradePositionData>& data = m_positionData[commodityID];
	//	//int i = 0;
	//	//for (; i < data.size(); i++) {
	//	//	data[i].positionCtrl->onTradeFinished(commodityID, type, price, volume, trend);
	//	//}
	//}
	if (m_orderPosCtrlRelation.find(orderID) != m_orderPosCtrlRelation.end() &&
		m_ordersSubmitted.find(orderID) != m_ordersSubmitted.end()) {
		UINT posCtrlID = m_orderPosCtrlRelation[orderID];
		APPositionCtrl* posCtrl = APPositionManager::getInstance()->getPositionCtrl(posCtrlID);
		if (posCtrl != NULL) {
			posCtrl->onTradeFinished(commodityID, type, price, volume, trend);
		}

		// 
		APTradeOrderInfo& orderInfo = m_ordersSubmitted[orderID];
		if (orderInfo.volume >= volume) {
			orderInfo.volume -= volume;
		}
		else {
			// error
			return;
		}

		if (orderInfo.volume == 0) {
			//trade fully finished
			m_orderPosCtrlRelation.erase(orderID);
			m_ordersSubmitted.erase(orderID);
		}
		else {
			return;
		}
	}
	else {
		std::list<APTradeOrderPositionInfo>::iterator it;
		for (it = m_quickDealOrders.begin(); it != m_quickDealOrders.end(); it++) {
			APTradeOrderPositionInfo& info = *it;
			if (info.orderInfo.commodityID == commodityID &&
				fabs(info.orderInfo.price - price) < DBL_EPSILON &&
				info.orderInfo.trend == trend &&
				info.orderInfo.type == type) {				
				UINT posCtrlID = info.positionCtrlID;
				APPositionCtrl* posCtrl = APPositionManager::getInstance()->getPositionCtrl(posCtrlID);
				if (posCtrl != NULL) {
					posCtrl->onTradeFinished(commodityID, type, price, volume, trend);
				}
			}
		}
	}
}

void APTrade::onTradeOrdered(APASSETID commodityID, APTradeType type, double price, long volume, APORDERID orderID, APTrendType trend) {
	std::list<APTradeOrderPositionInfo>::iterator it;
	for (it = m_ordersApplied.begin(); it != m_ordersApplied.end(); it++) {
		APTradeOrderPositionInfo& info = *it;
		if (info.orderInfo.commodityID == commodityID && 
			info.orderInfo.type == type &&
			fabs(info.orderInfo.price - price) < DBL_EPSILON && 
			info.orderInfo.volume == volume &&
			info.orderInfo.trend == trend &&
			info.orderInfo.orderID == UNDISTURBED_ORDER_ID) {
			info.orderInfo.orderID = orderID;
			m_ordersSubmitted[orderID] = info.orderInfo;
			m_orderPosCtrlRelation[orderID] = info.positionCtrlID;
			m_ordersApplied.erase(it);
			break;
		}
	}
}

void APTrade::onFundChanged(APASSETID commodityID, APTradeType type, double variableFund, APORDERID orderID, APTrendType trend) {
	//
}

std::vector<UINT> APTrade::getRelatedOrders(APPositionCtrl * pc)
{
	std::vector<UINT> orders;
	if (pc == NULL) {
		return orders;
	}

	UINT pcID = pc->getID();
	std::map<UINT, UINT>::iterator itor;
	for (itor = m_orderPosCtrlRelation.begin(); itor != m_orderPosCtrlRelation.end(); itor++) {
		UINT id = itor->second;
		if (pcID == id) {
			orders.push_back(itor->first);
		}
	}

	return orders;
}

//void APTrade::setPositionCtrl(APPositionCtrl * positionCtrl)
//{
//	m_positionCtrl = positionCtrl;
//}
