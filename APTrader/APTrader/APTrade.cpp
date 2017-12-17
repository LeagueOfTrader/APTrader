#include "APTrade.h"
#include "APPositionCtrl.h"
#include "APPositionManager.h"
#include "./Utils/APIntAccumulator.h"
#include "./Utils/APTimeUtility.h"

const UINT TRADE_ID_SHIFT = 1000;

APTrade::APTrade()
{
	m_idAccumulator = new APIntAccumulator();
}

APTrade::~APTrade()
{
	if (m_idAccumulator != NULL) {
		delete m_idAccumulator;
		m_idAccumulator = NULL;
	}
}

APORDERID APTrade::open(APASSETID instrumentID, APTrendType trend, double price, long volume, APPositionCtrl* pc, APOrderPriceType orderPriceType, APOrderTimeCondition ot) {
	if (pc == NULL) {
		return INVALID_TRADE_ORDER_ID;
	}

	APORDERID orderID = generateOrderID();
	if (ot == OTC_GoodForDay) {
		APTradeOrderPositionInfo info = { orderID, TDT_Open, instrumentID, price, volume, trend, pc->getID()};
		m_ordersApplied[orderID] = info;
	}
	else {
		APTradeOrderPositionInfo info = { orderID, TDT_Open, instrumentID, price, volume, trend, pc->getID() };
		m_quickDealOrders[orderID] = info;
	}
	
	open(orderID, instrumentID, trend, price, volume, orderPriceType, ot);
	return orderID;
}

APORDERID APTrade::close(APASSETID instrumentID, APTrendType trend, double price, long volume, APPositionCtrl* pc, APOrderPriceType orderPriceType, APOrderTimeCondition ot) {
	if (pc == NULL) {
		return INVALID_TRADE_ORDER_ID;
	}

	APORDERID orderID = generateOrderID();
	if (ot == OTC_GoodForDay) {
		APTradeOrderPositionInfo info = { UNDISTURBED_ORDER_ID, TDT_Close, instrumentID, price, volume, trend, pc->getID() };
		m_ordersApplied[orderID] = info;
	}
	else {
		APTradeOrderPositionInfo info = { MATCH_ANY_ORDER_ID, TDT_Close, instrumentID, price, volume, trend, pc->getID() };
		m_quickDealOrders[orderID] = info;
	}	
	
	close(orderID, instrumentID, trend, price, volume, orderPriceType, ot);
	return orderID;
}

void APTrade::cancel(APASSETID instrumentID, APTradeType type, APTrendType trend, double price, APPositionCtrl* pc) {
	if (pc == NULL) {
		return;
	}

	std::vector<UINT> relatedOrders = getRelatedOrders(pc);

	std::vector<UINT> cancelOrders;
	for (int i = 0; i < relatedOrders.size(); i++) {
		UINT id = relatedOrders[i];
		if (m_ordersConfirmed.find(id) != m_ordersConfirmed.end()) {
			APTradeOrderInfo& info = m_ordersConfirmed[id];
			if (info.instrumentID == instrumentID &&
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

void APTrade::cancel(APASSETID instrumentID, APTradeType type, APPositionCtrl * pc)
{
	if (pc == NULL) {
		return;
	}

	std::vector<UINT> relatedOrders = getRelatedOrders(pc);

	std::vector<UINT> cancelOrders;
	for (int i = 0; i < relatedOrders.size(); i++) {
		UINT id = relatedOrders[i];
		if (m_ordersConfirmed.find(id) != m_ordersConfirmed.end()) {
			APTradeOrderInfo& info = m_ordersConfirmed[id];
			if (info.instrumentID == instrumentID &&
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

void APTrade::cancelAll(APASSETID instrumentID, APPositionCtrl * pc)
{
	if (pc == NULL) {
		return;
	}

	std::vector<UINT> relatedOrders = getRelatedOrders(pc);

	std::vector<UINT> cancelOrders;
	for (int i = 0; i < relatedOrders.size(); i++) {
		UINT id = relatedOrders[i];
		if (m_ordersConfirmed.find(id) != m_ordersConfirmed.end()) {
			APTradeOrderInfo& info = m_ordersConfirmed[id];
			if (info.instrumentID == instrumentID) {
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

void APTrade::cancel(APORDERID orderID, APPositionCtrl * pc)
{
	std::map<APORDERID, APTradeOrderPositionInfo>::iterator itor = m_ordersApplied.find(orderID);
	if (itor != m_ordersApplied.end()) {
		m_ordersApplied.erase(orderID);
	}
	std::map<APORDERID, APTradeOrderInfo>::iterator it = m_ordersConfirmed.find(orderID);
	if (it != m_ordersConfirmed.end()) {
		APTradeOrderInfo& info = it->second;
		cancel(info.sysID);
	}
	//cancel(sysID);
}


void APTrade::onTradeDealt(APASSETID instrumentID, APTradeType type, double price, long volume, APORDERID orderID, 
							APTradeState state, APSYSTEMID sysID, APTrendType trend)
{
	if (m_orderPosCtrlRelation.find(orderID) != m_orderPosCtrlRelation.end() &&
		m_ordersConfirmed.find(orderID) != m_ordersConfirmed.end()) {
		UINT posCtrlID = m_orderPosCtrlRelation[orderID];

		APTradeOrderInfo& orderInfo = m_ordersConfirmed[orderID];
		long deltaVolume = orderInfo.volume - volume;
		orderInfo.volume = volume;
		orderInfo.sysID = sysID;
		orderInfo.state = state;

		APPositionCtrl* posCtrl = APPositionManager::getInstance()->getPositionCtrl(posCtrlID);
		if (posCtrl != NULL) {			
			//if (deltaVolume > 0) {
			posCtrl->onTradeDealt(instrumentID, type, price, deltaVolume, orderID, trend);
			//}
		}
		
		if (orderInfo.volume == 0) {
			//trade fully finished
			if (posCtrl != NULL) {
				posCtrl->onCompleteOrder(orderID, type);
			}
			m_orderPosCtrlRelation.erase(orderID);
			m_ordersConfirmed.erase(orderID);
		}
	}
	else {
		//
		std::map<APORDERID, APTradeOrderPositionInfo>::iterator it = m_quickDealOrders.find(orderID);
		if(it != m_quickDealOrders.end()) {
			APTradeOrderPositionInfo& info = it->second;
			long deltaVolume = info.orderInfo.volume - volume;
			
			UINT posCtrlID = info.positionCtrlID;
			APPositionCtrl* posCtrl = APPositionManager::getInstance()->getPositionCtrl(posCtrlID);
			if (posCtrl != NULL) {
				posCtrl->onTradeDealt(instrumentID, type, price, deltaVolume, trend);
			}
		}
	}
}

void APTrade::onTradeOrdered(APASSETID instrumentID, APTradeType type, double price, long volume, APORDERID orderID, APTradeState state, APSYSTEMID sysID, APTrendType trend) {
	std::map<APORDERID, APTradeOrderPositionInfo>::iterator it = m_ordersApplied.find(orderID);
	if(it != m_ordersApplied.end()) {
		APTradeOrderPositionInfo& info = it->second;
		info.orderInfo.sysID = sysID;
		m_ordersConfirmed[orderID] = info.orderInfo;
		m_orderPosCtrlRelation[orderID] = info.positionCtrlID;
		m_ordersApplied.erase(it);
	}
}

void APTrade::onFundChanged(APASSETID instrumentID, APTradeType type, double variableFund, APORDERID orderID, APTrendType trend) {
	//
}

bool APTrade::getOrderInfo(APORDERID orderID, APTradeOrderInfo& orderInfo)
{
	if (m_ordersConfirmed.find(orderID) != m_ordersConfirmed.end()) {
		orderInfo = m_ordersConfirmed[orderID];
		return true;
	}

	return false;
}

APORDERID APTrade::generateOrderID()
{
	UINT accumID = m_idAccumulator->generateID();
	APORDERID orderID = APTimeUtility::getTimestamp() * TRADE_ID_SHIFT + accumID % TRADE_ID_SHIFT;
	return orderID;
}

std::vector<APORDERID> APTrade::getRelatedOrders(APPositionCtrl * pc)
{
	std::vector<UINT> orders;
	if (pc == NULL) {
		return orders;
	}

	UINT pcID = pc->getID();
	std::map<APORDERID, UINT>::iterator itor;
	for (itor = m_orderPosCtrlRelation.begin(); itor != m_orderPosCtrlRelation.end(); itor++) {
		UINT id = itor->second;
		if (pcID == id) {
			orders.push_back(itor->first);
		}
	}

	return orders;
}
