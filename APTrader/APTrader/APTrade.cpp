#include "APTrade.h"
#include "APPositionCtrl.h"
#include "APPositionManager.h"
#include "Utils/APIntAccumulator.h"
#include "Utils/APTimeUtility.h"
#include "3rdParty/jsoncpp/include/json/writer.h"
#include "Utils/APJsonReader.h"
#include "APGlobalConfig.h"
#include "APPositionRepertory.h"
#include "APMacro.h"
#include "Utils/APRedisAgent.h"
//#include <math.h>

#ifdef USE_CTP
#include "Impl/CTP/APFuturesCTPTraderAgent.h"
#endif

const UINT TRADE_ID_SHIFT = 1000000;

APTrade::APTrade()
{
	m_idAccumulator = new APIntAccumulator();
	m_inited = false;
	m_baseID = "";
}

APTrade::~APTrade()
{
	if (m_idAccumulator != NULL) {
		delete m_idAccumulator;
		m_idAccumulator = NULL;
	}
}

APORDERID APTrade::open(APASSETID instrumentID, APTradeDirection direction, double price, long volume, APPositionCtrl* pc, APOrderTimeCondition ot) {
	
	//APORDERID orderID = generateOrderID();
	//APTradeOrderInfo info = { orderID, TT_Open, instrumentID, price, volume, direction, OS_None, pc->getID()};
	//m_localOrders[orderID] = info;
	//
	//if (APGlobalConfig::getInstance()->useRepertory()) {
	//	long vol = APPositionRepertory::getInstance()->distribute(instrumentID, TT_Open, direction, volume);
	//	if (vol > 0) {
	//		pc->onTradeDealt(instrumentID, TT_Open, price, volume, orderID, direction);
	//		volume -= vol;
	//	}		
	//}
	APORDERID orderID = initializeLocalOrder(instrumentID, TT_Open, direction, price, volume, pc);

	if (orderID == INVALID_ORDER_ID) {
		return orderID;
	}

	open(orderID, instrumentID, direction, price, volume, ot);
	return orderID;
}

APORDERID APTrade::close(APASSETID instrumentID, APTradeDirection direction, double price, long volume, APPositionCtrl* pc, APOrderTimeCondition ot) {
	//if (pc == NULL) {
	//	return INVALID_ORDER_ID;
	//}

	//APORDERID orderID = generateOrderID();
	//APTradeOrderInfo info = { orderID, TT_Close, instrumentID, price, volume, direction, OS_None, pc->getID() };
	//m_localOrders[orderID] = info;

	//if (APGlobalConfig::getInstance()->useRepertory()) {
	//	long vol = APPositionRepertory::getInstance()->distribute(instrumentID, TT_Close, direction, volume);
	//	if (vol > 0) {
	//		pc->onTradeDealt(instrumentID, TT_Close, price, volume, orderID, direction);
	//		volume -= vol;
	//	}
	//}
	APORDERID orderID = initializeLocalOrder(instrumentID, TT_Close, direction, price, volume, pc);

	if (orderID == INVALID_ORDER_ID) {
		return orderID;
	}
	
	close(orderID, instrumentID, direction, price, volume,  ot);
	return orderID;
}

APORDERID APTrade::open(APASSETID instrumentID, APTradeDirection direction, APOrderPriceType orderPriceType, double price, APPositionCtrl * pc, APOrderTimeCondition orderTimeCondition, std::string date, APOrderVolumeCondition orderVolumeCondition, long volume, long minVolume, APOrderContingentCondition orderContingentCondition, double stopPrice)
{
	//if (pc == NULL) {
	//	return INVALID_ORDER_ID;
	//}
	//APORDERID orderID = generateOrderID();
	//APTradeOrderInfo info = { orderID, TT_Open, instrumentID, price, volume, direction, OS_None, pc->getID() };
	//m_localOrders[orderID] = info;

	APORDERID orderID = initializeLocalOrder(instrumentID, TT_Open, direction, price, volume, pc);

	if (orderID == INVALID_ORDER_ID) {
		return orderID;
	}

	open(instrumentID, orderID, direction, 
		orderPriceType, price,
		orderTimeCondition, date,
		orderVolumeCondition, volume, minVolume,
		orderContingentCondition, stopPrice);
	return orderID;
}

APORDERID APTrade::close(APASSETID instrumentID, APTradeDirection direction, APOrderPriceType orderPriceType, double price, APPositionCtrl * pc, APOrderTimeCondition orderTimeCondition, std::string date, APOrderVolumeCondition orderVolumeCondition, long volume, long minVolume, APOrderContingentCondition orderContingentCondition, double stopPrice)
{
	//if (pc == NULL) {
	//	return INVALID_ORDER_ID;
	//}
	//APORDERID orderID = generateOrderID();
	//APTradeOrderInfo info = { orderID, TT_Close, instrumentID, price, volume, direction, OS_None, pc->getID() };
	//m_localOrders[orderID] = info;

	APORDERID orderID = initializeLocalOrder(instrumentID, TT_Close, direction, price, volume, pc);

	if (orderID == INVALID_ORDER_ID) {
		return orderID;
	}

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

	std::vector<APORDERID> relatedOrders = getRelatedOrders(pc);

	std::vector<APORDERID> cancelOrders;
	for (int i = 0; i < relatedOrders.size(); i++) {
		APORDERID id = relatedOrders[i];
		if (m_localOrders.find(id) != m_localOrders.end()) {
			APTradeOrderInfo& info = m_localOrders[id];
			if (info.instrumentID == instrumentID &&
				info.type == type &&
				info.direction == direction) {
				if ( fabs(info.price - price) < DBL_EPSILON ||
					(((direction == TD_Buy && type == TT_Close) || (direction == TD_Sell && type == TT_Open)) && info.price > price) ||  // Long Close / Short Open, cancel higher offered price
					(((direction == TD_Sell && type == TT_Close) || (direction == TD_Buy && type == TT_Open)) && info.price < price) ) { // Long Open / Short Close, cancel lower offered price
					APORDERID orderID = info.orderID;
					cancelOrders.push_back(orderID);
				}
			}
		}
	}

	for (int i = 0; i < cancelOrders.size(); i++) {
		APORDERID orderID = cancelOrders[i];
		if (orderID != INVALID_ORDER_ID) {
			cancel(orderID);
		}
	}
}

void APTrade::cancel(APASSETID instrumentID, APTradeType type, APPositionCtrl * pc)
{
	if (pc == NULL) {
		return;
	}

	std::vector<APORDERID> relatedOrders = getRelatedOrders(pc);

	std::vector<APORDERID> cancelOrders;
	for (int i = 0; i < relatedOrders.size(); i++) {
		APORDERID id = relatedOrders[i];
		if (m_localOrders.find(id) != m_localOrders.end()) {
			APTradeOrderInfo& info = m_localOrders[id];
			if (info.instrumentID == instrumentID &&
				info.type == type) {
				APORDERID orderID = info.orderID;
				cancelOrders.push_back(orderID);
			}
		}
	}

	for (int i = 0; i < cancelOrders.size(); i++) {
		APORDERID orderID = cancelOrders[i];
		if (orderID != INVALID_ORDER_ID) {
			cancel(orderID);
		}
	}
}

void APTrade::cancelAll(APASSETID instrumentID, APPositionCtrl * pc)
{
	if (pc == NULL) {
		return;
	}

	std::vector<APORDERID> relatedOrders = getRelatedOrders(pc);

	std::vector<APORDERID> cancelOrders;
	for (int i = 0; i < relatedOrders.size(); i++) {
		APORDERID id = relatedOrders[i];
		if (m_localOrders.find(id) != m_localOrders.end()) {
			APTradeOrderInfo& info = m_localOrders[id];
			if (info.instrumentID == instrumentID) {
				APORDERID orderID = info.orderID;
				cancelOrders.push_back(orderID);
			}
		}
	}

	for (int i = 0; i < cancelOrders.size(); i++) {
		APORDERID orderID = cancelOrders[i];
		if (orderID != INVALID_ORDER_ID) {
			cancel(orderID);
		}
	}
}

void APTrade::cancel(APORDERID orderID, APPositionCtrl * pc)
{
	cancel(orderID);
}

void APTrade::onTraded(APASSETID instrumentID, APTradeType type, double price, long volume, 
						APORDERID orderID, APTradeDirection direction)
{
	if (m_localOrders.find(orderID) != m_localOrders.end()) {		
		APTradeOrderInfo& orderInfo = m_localOrders[orderID];

		APPositionCtrl* posCtrl = getPositionCtrlByOrder(orderID);
		if (posCtrl != NULL) {			
			posCtrl->onTradeDealt(instrumentID, type, price, volume, orderID, direction);
		}

		if (orderInfo.state == OS_AllTraded || orderInfo.state == OS_PartTradedNotQueueing) {
			if (posCtrl != NULL) {
				posCtrl->onCompleteOrder(orderID, type);
			}
			removeLocalOrder(orderID);
		}
	}
}

void APTrade::onOrderStatusChanged(APASSETID instrumentID, APTradeType type, APORDERID orderID, long volumeSurplus, long volumeTraded, 
									APOrderState state, APSYSTEMID sysID, APSYSTEMID orderRef, 
									APSYSTEMID exchangeID, int sessionID, int frontID,
									APTradeDirection direction) {
	std::map<APORDERID, APTradeOrderInfo>::iterator it = m_localOrders.find(orderID);
	if(it != m_localOrders.end()) {
		APTradeOrderInfo& info = it->second;
		info.sysID = sysID;
		info.state = state;
		info.exchangeID = exchangeID;
		info.sessionID = sessionID;
		info.orderRef = orderRef;
		info.volumeSurplus = volumeSurplus;
		info.volumeTraded = volumeTraded;

		if (state == OS_Canceled || state == OS_NoTradeNotQueueing) {
			onCanceled(orderID);
		}
	}
}

void APTrade::onCanceled(APORDERID orderID)
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

void APTrade::setOrderIDBase(UINT base)
{
	m_idAccumulator->setBase(base);
}

void APTrade::queryAllOrders()
{
#ifdef USE_CTP
	APFuturesCTPTraderAgent::getInstance()->reqQryAllOrders();
	//APFuturesCTPTraderAgent::getInstance()->reqQryLastOrders();
#endif
}

void APTrade::bindOrder(APORDERID localOrderID, APPositionCtrl * posCtrl)
{
	if (posCtrl == NULL) {
		return;
	}

	if (m_localOrders.find(localOrderID) != m_localOrders.end()) {
		m_localOrders[localOrderID].positionCtrlID = posCtrl->getID();
	}
}

void APTrade::queryOrder(APORDERID localOrderID)
{
#ifdef USE_CTP
	if (m_localOrders.find(localOrderID) == m_localOrders.end()) {
		return;
	}
	APTradeOrderInfo& oi = m_localOrders[localOrderID];
	APFuturesCTPTraderAgent::getInstance()->reqQryOrder(oi.instrumentID, oi.sysID, oi.exchangeID);
#endif
}

void APTrade::onQueryOrder(APORDERID localOrderID)
{
#ifdef USE_CTP
	APTradeOrderInfo toi;
	APFuturesCTPTraderAgent::getInstance()->getOrderInfo(localOrderID, toi);
	m_localOrders[localOrderID] = toi;
#endif
}

void APTrade::onQueryOrderFailed(APORDERID localOrderID)
{
	//
}

void APTrade::onSyncOrders()
{
	setInited();
}

void APTrade::init()
{
	std::string dateStr = APTimeUtility::getDate();
	//UINT base = atoi(dateStr.substr(0, 8).c_str()); //APTimeUtility::getTimestamp() * 1000;
	//m_idAccumulator->setBase(base);
	m_baseID = dateStr.substr(2, 6); //dateStr.substr(0, 8);
#ifdef USE_CTP
	//int maxOrderRef = APFuturesCTPTraderAgent::getInstance()->getMaxOrderRef();
	//m_idAccumulator->setBase(maxOrderRef);
#endif // USE_CTP
	bool newDay = false;
	if (APRedisAgent::getInstance()->hasKey("TradeDate")) {
		std::string strLastDate = APRedisAgent::getInstance()->read("TradeDate");
		if (APTimeUtility::compareDate(dateStr, strLastDate) != 0) {
			newDay = true;
			m_idAccumulator->setBase(0);
		}
	}
	if (!newDay) {
		if (APRedisAgent::getInstance()->hasKey("TradeID")) {
			std::string strID = APRedisAgent::getInstance()->read("TradeID");
			UINT lastID = atoi(strID.c_str());
			if (++lastID >= TRADE_ID_SHIFT) {
				lastID = 1; // turn back to 1
			}
			m_idAccumulator->setBase(lastID);
		}
	}
}

APORDERID APTrade::generateOrderID()
{
	UINT accumID = m_idAccumulator->generateID();
	saveOrderAccumID(accumID);

	char strAccumID[8];
	sprintf(strAccumID, "%06d", accumID);

	APORDERID orderID = m_baseID + strAccumID;// % TRADE_ID_SHIFT;//accumID;//
	return orderID;
}

void APTrade::removeLocalOrder(APORDERID orderID)
{
	//m_orderPosCtrlRelation.erase(orderID);
	m_localOrders.erase(orderID);
}

bool APTrade::isOrderExists(APORDERID orderID)
{
	return (m_localOrders.find(orderID) != m_localOrders.end());
}

APASSETID APTrade::getInstrumentIDByOrderID(APORDERID orderID)
{
	APASSETID instrumentID;
	if (m_localOrders.find(orderID) != m_localOrders.end()) {
		instrumentID = m_localOrders[orderID].instrumentID;
	}
	return instrumentID;
}

APSYSTEMID APTrade::getSysIDByOrderID(APORDERID orderID)
{
	APSYSTEMID sysID = "";
	if (m_localOrders.find(orderID) != m_localOrders.end()) {
		sysID = m_localOrders[orderID].sysID;
	}
	return sysID;
}

APSYSTEMID APTrade::getOrderRefByOrderID(APORDERID orderID)
{
	APSYSTEMID orderRef = "";
	if (m_localOrders.find(orderID) != m_localOrders.end()) {
		orderRef = m_localOrders[orderID].orderRef;
	}
	return orderRef;
}

int APTrade::getSessionIDByOrderID(APORDERID orderID)
{
	int sessionID = 0;
	if (m_localOrders.find(orderID) != m_localOrders.end()) {
		sessionID = m_localOrders[orderID].sessionID;
	}
	return sessionID;
}

int APTrade::getFrontIDByOrderID(APORDERID orderID)
{
	int frontID = 0;
	if (m_localOrders.find(orderID) != m_localOrders.end()) {
		frontID = m_localOrders[orderID].frontID;
	}
	return frontID;
}

APSYSTEMID APTrade::getExchangeIDByOrderID(APORDERID orderID)
{
	APSYSTEMID exchangeID = "";
	if (m_localOrders.find(orderID) != m_localOrders.end()) {
		exchangeID = m_localOrders[orderID].exchangeID;
	}
	return exchangeID;
}

APORDERID APTrade::initializeLocalOrder(APASSETID instrumentID, APTradeType type, APTradeDirection direction, double price, long volume, APPositionCtrl * pc)
{
	if (pc == NULL) {
		return INVALID_ORDER_ID;
	}

	APORDERID orderID = generateOrderID();
	APTradeOrderInfo info = { orderID, type, instrumentID, price, volume, direction, OS_None, pc->getID() };
	m_localOrders[orderID] = info;

	if (APGlobalConfig::getInstance()->useRepertory()) {
		long vol = APPositionRepertory::getInstance()->distribute(instrumentID, type, direction, volume);
		if (vol > 0) {
			pc->onTradeDealt(instrumentID, type, price, volume, orderID, direction);
			volume -= vol;
		}
	}

	return orderID;
}

//std::string APTrade::serialize()
//{
//	Json::Value v;
//	Json::Value arr;
//	v["LocalOrders"] = arr;
//	std::map<APORDERID, APTradeOrderInfo>::iterator it;
//	int index = 0;
//	for (it = m_localOrders.begin(); it != m_localOrders.end(); it++) {
//		APTradeOrderInfo& oi = it->second;
//		APOrderRecordInfo ori = convertOrderInfo(oi);
//		Json::Value rec;
//		rec["Direction"] = (int)ori.direction;
//		rec["LocalID"] = ori.localID;
//		rec["InstrumentID"] = ori.instrumentID;
//		rec["RecordTime"] = ori.recordTime;
//		rec["State"] = (int)ori.state;
//		rec["SysID"] = ori.sysID;
//		rec["OrderRef"] = ori.orderRef;
//		rec["ExchangeID"] = ori.exchangeID;
//		rec["FrontID"] = ori.frontID;
//		rec["SessionID"] = ori.sessionID;
//		rec["VolumeSurplus"] = ori.volumeSurplus;
//		arr[index++] = rec;
//	}
//	return std::string();
//}
//
//void APTrade::deserialize(std::string str)
//{
//	APJsonReader jr;
//	jr.initWithString(str);
//
//	m_orderRecordInfo.clear();
//
//	int count = jr.getArraySize("LocalOrders");
//	for (int i = 0; i < count; i++) {
//		APOrderRecordInfo ori;
//		memset(&ori, 0, sizeof(ori));
//		std::string ordStr = jr.getArrayObjValue("LocalOrders", i);
//		Json::Reader recReader;
//		
//		Json::Value rec;
//		recReader.parse(ordStr, rec);
//
//		ori.direction = (APTradeDirection)rec["Direction"].asInt();
//		ori.localID = rec["LocalID"].asInt();
//		ori.instrumentID = rec["InstrumentID"].asString();
//		ori.recordTime = rec["RecordTime"].asString();
//		ori.state = (APOrderState)rec["State"].asInt();
//		ori.sysID = rec["SysID"].asString();
//		ori.orderRef = rec["OrderRef"].asString();
//		ori.exchangeID = rec["ExchangeID"].asString();
//		ori.frontID = rec["FrontID"].asInt();
//		ori.sessionID = rec["SessionID"].asInt();
//		ori.volumeSurplus = rec["VolumeSurplus"].asInt();
//
//		m_orderRecordInfo[ori.localID] = ori;
//	}
//}
//
//std::string APTrade::generateRedisKey()
//{
//	std::string redisKey = APAccountInfo::getInstance()->getInterfaceType() + ":"
//		+ APAccountInfo::getInstance()->getAccountID() + ":Trade:RecordOrders";
//
//	return redisKey;
//}
//
//APOrderRecordInfo APTrade::convertOrderInfo(APTradeOrderInfo & info)
//{
//	APOrderRecordInfo record;
//	memset(&record, 0, sizeof(record));
//
//	record.direction = info.direction;
//	record.instrumentID = info.instrumentID;
//	record.localID = info.orderID;
//	record.recordTime = APTimeUtility::getDateTime();
//	record.state = info.state;
//	record.sysID = info.sysID;
//	record.exchangeID = info.exchangeID;
//	record.frontID = info.frontID;
//	record.sessionID = info.sessionID;
//	record.orderRef = info.orderRef;
//	record.volumeSurplus = info.volumeSurplus;
//	
//	return record;
//}

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

void APTrade::saveOrderAccumID(UINT accumID)
{
	std::string today = APTimeUtility::getDate();
	char buf[8];
	sprintf(buf, "%d", accumID);
	std::string strAccumID = buf;
	APRedisAgent::getInstance()->writeAsync("TradeID", strAccumID);
	APRedisAgent::getInstance()->writeAsync("TradeDate", today);
}
