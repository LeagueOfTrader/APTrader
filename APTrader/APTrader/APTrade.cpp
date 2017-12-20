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

APORDERID APTrade::open(APASSETID instrumentID, APTrendType trend, double price, long volume, APPositionCtrl* pc, APOrderTimeCondition ot) {
	if (pc == NULL) {
		return INVALID_TRADE_ORDER_ID;
	}

	APORDERID orderID = generateOrderID();
	APTradeOrderInfo info = { orderID, TDT_Open, instrumentID, price, volume, trend, TS_New, 0, pc->getID()};
	m_localOrders[orderID] = info;
	//m_orderPosCtrlRelation[orderID] = pc->getID();
	
	open(orderID, instrumentID, trend, price, volume, ot);
	return orderID;
}

APORDERID APTrade::close(APASSETID instrumentID, APTrendType trend, double price, long volume, APPositionCtrl* pc, APOrderTimeCondition ot) {
	if (pc == NULL) {
		return INVALID_TRADE_ORDER_ID;
	}

	APORDERID orderID = generateOrderID();
	APTradeOrderInfo info = { orderID, TDT_Close, instrumentID, price, volume, trend, TS_New, 0, pc->getID() };
	m_localOrders[orderID] = info;
	//m_orderPosCtrlRelation[orderID] = pc->getID();
	
	close(orderID, instrumentID, trend, price, volume,  ot);
	return orderID;
}

APORDERID APTrade::open(APASSETID instrumentID, APTrendType trend, APOrderPriceType orderPriceType, double price, APPositionCtrl * pc, APOrderTimeCondition orderTimeCondition, std::string date, APOrderVolumeCondition orderVolumeCondition, long volume, long minVolume, APOrderContingentCondition orderContingentCondition, double stopPrice)
{
	if (pc == NULL) {
		return INVALID_TRADE_ORDER_ID;
	}

	APORDERID orderID = generateOrderID();
	APTradeOrderInfo info = { orderID, TDT_Open, instrumentID, price, volume, trend, TS_New, 0, pc->getID() };
	m_localOrders[orderID] = info;

	open(instrumentID, orderID, trend, 
		orderPriceType, price,
		orderTimeCondition, date,
		orderVolumeCondition, volume, minVolume,
		orderContingentCondition, stopPrice);
	return orderID;
}

APORDERID APTrade::close(APASSETID instrumentID, APTrendType trend, APOrderPriceType orderPriceType, double price, APPositionCtrl * pc, APOrderTimeCondition orderTimeCondition, std::string date, APOrderVolumeCondition orderVolumeCondition, long volume, long minVolume, APOrderContingentCondition orderContingentCondition, double stopPrice)
{
	if (pc == NULL) {
		return INVALID_TRADE_ORDER_ID;
	}

	APORDERID orderID = generateOrderID();
	APTradeOrderInfo info = { orderID, TDT_Close, instrumentID, price, volume, trend, TS_New, 0, pc->getID() };
	m_localOrders[orderID] = info;

	close(instrumentID, orderID, trend,
		orderPriceType, price,
		orderTimeCondition, date,
		orderVolumeCondition, volume, minVolume,
		orderContingentCondition, stopPrice);
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
		if (m_localOrders.find(id) != m_localOrders.end()) {
			APTradeOrderInfo& info = m_localOrders[id];
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
		if (m_localOrders.find(id) != m_localOrders.end()) {
			APTradeOrderInfo& info = m_localOrders[id];
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
		if (m_localOrders.find(id) != m_localOrders.end()) {
			APTradeOrderInfo& info = m_localOrders[id];
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
	cancel(orderID);
}

void APTrade::onTradeDealt(APASSETID instrumentID, APTradeType type, double price, long volume, APORDERID orderID, 
							APTradeState state, APSYSTEMID sysID, APTrendType trend)
{
	if (m_localOrders.find(orderID) != m_localOrders.end()) {		
		APTradeOrderInfo& orderInfo = m_localOrders[orderID];
		long deltaVolume = orderInfo.volume - volume;
		orderInfo.volume = volume;
		orderInfo.sysID = sysID;
		orderInfo.state = state;

		APPositionCtrl* posCtrl = getPositionCtrlByOrder(orderID);
		if (posCtrl != NULL) {			
			posCtrl->onTradeDealt(instrumentID, type, price, deltaVolume, orderID, trend);
		}
		
		if (orderInfo.volume == 0) {
			//trade fully finished
			if (posCtrl != NULL) {
				posCtrl->onCompleteOrder(orderID, type);
			}
			removeLocalOrder(orderID);
		}
		else {
			orderInfo.state = TS_PartiallyFilled;
		}
	}
}

void APTrade::onTradeOrdered(APASSETID instrumentID, APTradeType type, double price, long volume, APORDERID orderID, APTradeState state, APSYSTEMID sysID, APTrendType trend) {
	std::map<APORDERID, APTradeOrderInfo>::iterator it = m_localOrders.find(orderID);
	if(it != m_localOrders.end()) {
		APTradeOrderInfo& info = it->second;
		info.sysID = sysID;
		info.state = TS_Ordered;
	}
}

void APTrade::onTradeCanceled(APORDERID orderID, APSYSTEMID sysID)
{
	if (m_localOrders.find(orderID) != m_localOrders.end()) {
		APTradeOrderInfo& info = m_localOrders[orderID];
		APPositionCtrl* posCtrl = getPositionCtrlByOrder(orderID);
		if (posCtrl != NULL) {
			posCtrl->onTradeCanceled(info.instrumentID, info.type, info.volume, orderID, info.trend);
		}

		removeLocalOrder(orderID);
	}
}

void APTrade::onFundChanged(APASSETID instrumentID, APTradeType type, double variableFund, APORDERID orderID, APTrendType trend) {
	//
}

bool APTrade::getOrderInfo(APORDERID orderID, APTradeOrderInfo& orderInfo)
{
	if (m_localOrders.find(orderID) != m_localOrders.end()) {
		orderInfo = m_localOrders[orderID];
		return true;
	}

	return false;
}

void APTrade::setOrderIDBase(APORDERID base)
{
	m_idAccumulator->setBase(base);
}

APORDERID APTrade::generateOrderID()
{
	UINT accumID = m_idAccumulator->generateID();
	APORDERID orderID = accumID;//APTimeUtility::getTimestamp() * TRADE_ID_SHIFT + accumID % TRADE_ID_SHIFT;
	return orderID;
}

void APTrade::removeLocalOrder(APORDERID orderID)
{
	//m_orderPosCtrlRelation.erase(orderID);
	m_localOrders.erase(orderID);
}

APSYSTEMID APTrade::getSysIDByOrder(APORDERID orderID)
{
	APSYSTEMID sysID = 0;
	if (m_localOrders.find(orderID) != m_localOrders.end()) {
		sysID = m_localOrders[orderID].sysID;
	}
	return sysID;
}

std::vector<APORDERID> APTrade::getRelatedOrders(APPositionCtrl * pc)
{
	std::vector<APORDERID> orders;
	if (pc == NULL) {
		return orders;
	}

	UINT pcID = pc->getID();
	std::map<APORDERID, APTradeOrderInfo>::iterator it;
	for (it = m_localOrders.begin(); it != m_localOrders.end(); it++) {
		APTradeOrderInfo& info = it->second;
		if (pcID == info.positionCtrlID) {
			orders.push_back(it->first);
		}
	}

	return orders;
}

APPositionCtrl * APTrade::getPositionCtrlByOrder(APORDERID orderID)
{
	if (m_localOrders.find(orderID) != m_localOrders.end()) {
		UINT posCtrlID = m_localOrders[orderID].positionCtrlID;
		APPositionCtrl* posCtrl = APPositionManager::getInstance()->getPositionCtrl(posCtrlID);
		return posCtrl;
	}
	return NULL;
}
