#include "APTrade.h"
#include "APPositionCtrl.h"
#include "APPositionManager.h"
#include "./Utils/APIntAccumulator.h"
#include "./Utils/APTimeUtility.h"

#include "APMacro.h"

#ifdef USE_CTP
#include "Impl/CTP/APFuturesCTPTraderAgent.h"
#endif

//const UINT TRADE_ID_SHIFT = 1000;

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

APORDERID APTrade::open(APASSETID instrumentID, APTradeDirection direction, double price, long volume, APPositionCtrl* pc, APOrderTimeCondition ot) {
	if (pc == NULL) {
		return INVALID_TRADE_ORDER_ID;
	}

	APORDERID orderID = generateOrderID();
	APTradeOrderInfo info = { orderID, TT_Open, instrumentID, price, volume, direction, OS_None, 0, pc->getID()};
	m_localOrders[orderID] = info;
	//m_orderPosCtrlRelation[orderID] = pc->getID();
	
	open(orderID, instrumentID, direction, price, volume, ot);
	return orderID;
}

APORDERID APTrade::close(APASSETID instrumentID, APTradeDirection direction, double price, long volume, APPositionCtrl* pc, APOrderTimeCondition ot) {
	if (pc == NULL) {
		return INVALID_TRADE_ORDER_ID;
	}

	APORDERID orderID = generateOrderID();
	APTradeOrderInfo info = { orderID, TT_Close, instrumentID, price, volume, direction, OS_None, 0, pc->getID() };
	m_localOrders[orderID] = info;
	//m_orderPosCtrlRelation[orderID] = pc->getID();
	
	close(orderID, instrumentID, direction, price, volume,  ot);
	return orderID;
}

APORDERID APTrade::open(APASSETID instrumentID, APTradeDirection direction, APOrderPriceType orderPriceType, double price, APPositionCtrl * pc, APOrderTimeCondition orderTimeCondition, std::string date, APOrderVolumeCondition orderVolumeCondition, long volume, long minVolume, APOrderContingentCondition orderContingentCondition, double stopPrice)
{
	if (pc == NULL) {
		return INVALID_TRADE_ORDER_ID;
	}

	APORDERID orderID = generateOrderID();
	APTradeOrderInfo info = { orderID, TT_Open, instrumentID, price, volume, direction, OS_None, 0, pc->getID() };
	m_localOrders[orderID] = info;

	open(instrumentID, orderID, direction, 
		orderPriceType, price,
		orderTimeCondition, date,
		orderVolumeCondition, volume, minVolume,
		orderContingentCondition, stopPrice);
	return orderID;
}

APORDERID APTrade::close(APASSETID instrumentID, APTradeDirection direction, APOrderPriceType orderPriceType, double price, APPositionCtrl * pc, APOrderTimeCondition orderTimeCondition, std::string date, APOrderVolumeCondition orderVolumeCondition, long volume, long minVolume, APOrderContingentCondition orderContingentCondition, double stopPrice)
{
	if (pc == NULL) {
		return INVALID_TRADE_ORDER_ID;
	}

	APORDERID orderID = generateOrderID();
	APTradeOrderInfo info = { orderID, TT_Close, instrumentID, price, volume, direction, OS_None, 0, pc->getID() };
	m_localOrders[orderID] = info;

	close(instrumentID, orderID, direction,
		orderPriceType, price,
		orderTimeCondition, date,
		orderVolumeCondition, volume, minVolume,
		orderContingentCondition, stopPrice);
	return orderID;
}

void APTrade::cancel(APASSETID instrumentID, APTradeType type, APTradeDirection direction, double price, APPositionCtrl* pc) {
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
				info.direction == direction) {
				if ( fabs(info.price - price) < DBL_EPSILON ||
					(((direction == TD_Buy && type == TT_Close) || (direction == TD_Sell && type == TT_Open)) && info.price > price) ||  // Long Close / Short Open, cancel higher offered price
					(((direction == TD_Sell && type == TT_Close) || (direction == TD_Buy && type == TT_Open)) && info.price < price) ) { // Long Open / Short Close, cancel lower offered price
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

void APTrade::onTraded(APASSETID instrumentID, APTradeType type, double price, long volume, APORDERID orderID, 
							APSYSTEMID sysID, APTradeDirection direction)
{
	if (m_localOrders.find(orderID) != m_localOrders.end()) {		
		APTradeOrderInfo& orderInfo = m_localOrders[orderID];

		APPositionCtrl* posCtrl = getPositionCtrlByOrder(orderID);
		if (posCtrl != NULL) {			
			posCtrl->onTradeDealt(instrumentID, type, price, volume, orderID, direction);
		}
	}
}

void APTrade::onOrderStatusChanged(APASSETID instrumentID, APTradeType type, APORDERID orderID, long volumeSurplus, long volumeTraded, 
									APOrderState state, APSYSTEMID sysID, APSYSTEMID tradeID, APTradeDirection direction) {
	std::map<APORDERID, APTradeOrderInfo>::iterator it = m_localOrders.find(orderID);
	if(it != m_localOrders.end()) {
		APTradeOrderInfo& info = it->second;
		info.sysID = sysID;
		info.state = state;
		info.tradeID = tradeID;
		info.volumeSurplus = volumeSurplus;
		info.volumeTraded = volumeTraded;

		//
		APPositionCtrl* posCtrl = getPositionCtrlByOrder(orderID);
		if (state == OS_AllTraded || state == OS_PartTradedNotQueueing) {
			if (posCtrl != NULL) {
				posCtrl->onCompleteOrder(orderID, type);
			}
			removeLocalOrder(orderID);
		}

		if (state == OS_Canceled || state == OS_NoTradeNotQueueing) {
			onCanceled(orderID, sysID);
		}
	}
}

void APTrade::onCanceled(APORDERID orderID, APSYSTEMID sysID)
{
	if (m_localOrders.find(orderID) != m_localOrders.end()) {
		APTradeOrderInfo& info = m_localOrders[orderID];
		APPositionCtrl* posCtrl = getPositionCtrlByOrder(orderID);
		if (posCtrl != NULL) {
			posCtrl->onTradeCanceled(info.instrumentID, info.type, info.volume, orderID, info.direction);
		}

		removeLocalOrder(orderID);
	}
}

void APTrade::onFailed(APORDERID orderID) 
{
	onCanceled(orderID);
}

void APTrade::onFundChanged(APASSETID instrumentID, APTradeType type, double variableFund, APORDERID orderID, APTradeDirection direction) {
	// todo: implement later
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

void APTrade::bindOrder(APORDERID localOrderID, APPositionCtrl * posCtrl)
{
	if (posCtrl == NULL) {
		return;
	}

	if (m_orderRecordInfo.find(localOrderID) != m_orderRecordInfo.end()) {
		m_orderRecordInfo[localOrderID].positionCtrlID = posCtrl->getID();
	}
}

void APTrade::queryOrder(APORDERID localOrderID)
{
#ifdef USE_CTP
	if (m_orderRecordInfo.find(localOrderID) == m_orderRecordInfo.end()) {
		return;
	}
	APOrderRecordInfo& ori = m_orderRecordInfo[localOrderID];
	APFuturesCTPTraderAgent::getInstance()->reqQryOrder(ori.instrumentID, ori.sysID);
#endif
}

void APTrade::onQueryOrder(APORDERID localOrderID)
{
#ifdef USE_CTP
	if (m_orderRecordInfo.find(localOrderID) != m_orderRecordInfo.end()) {
		m_orderRecordInfo.erase(localOrderID);
	}
	
	APOrderRecordInfo& ori = m_orderRecordInfo[localOrderID];
	APTradeOrderInfo toi = APFuturesCTPTraderAgent::getInstance()->getOrderInfo(localOrderID);
	toi.positionCtrlID = ori.positionCtrlID;
	m_localOrders[localOrderID] = toi;	

	bool orderChanged = false;
	if (ori.state != toi.state) {
		orderChanged = true;
	}
	else if (ori.volumeSurplus != toi.volumeSurplus) {
		orderChanged = true;
	}

	if (orderChanged) {
		// do not query trade, use order comparision first
		long deltaVolume = ori.volumeSurplus - toi.volumeSurplus;
		if (deltaVolume > 0) {
			onTraded(toi.instrumentID, toi.type, toi.price, deltaVolume, localOrderID, toi.sysID, toi.direction);
		}
		// query trade
		//std::string curDate = APTimeUtility::getDate();
		//APFuturesCTPTraderAgent::getInstance()->reqQryTrade(toi.tradeID, APTimeUtility::getDateInDateTime(ori.recordTime), curDate);
		onOrderStatusChanged(toi.instrumentID, toi.type, localOrderID, toi.volumeSurplus, toi.volumeTraded, 
							toi.state, toi.sysID, toi.tradeID, toi.direction);
	}
	else {
		if (toi.state == OS_AllTraded || toi.state == OS_PartTradedNotQueueing) {
			APPositionCtrl* posCtrl = getPositionCtrlByOrder(localOrderID);
			if (posCtrl != NULL) {
				posCtrl->onCompleteOrder(localOrderID, toi.type);
			}
			removeLocalOrder(localOrderID);
		}
	}
#endif
}

void APTrade::onQueryOrderFailed(APORDERID localOrderID)
{
	if (m_orderRecordInfo.find(localOrderID) != m_orderRecordInfo.end()) {
		APOrderRecordInfo& ori = m_orderRecordInfo[localOrderID];
		APPositionCtrl* posCtrl = getPositionCtrlByOrder(ori.positionCtrlID);
		if (posCtrl != NULL) {
			posCtrl->onOrderOutdated(localOrderID);
		}
		
		m_orderRecordInfo.erase(localOrderID);
	}
}

bool APTrade::isOrderDataComplete()
{
	return m_orderRecordInfo.size() == m_localOrders.size();
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
