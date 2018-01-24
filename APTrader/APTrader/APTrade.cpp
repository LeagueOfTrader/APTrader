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
		return INVALID_ORDER_ID;
	}

	APORDERID orderID = generateOrderID();
	APTradeOrderInfo info = { orderID, TT_Open, instrumentID, price, volume, direction, OS_None, "0", pc->getID()};
	m_localOrders[orderID] = info;
	
	if (APGlobalConfig::getInstance()->useRepertory()) {
		long vol = APPositionRepertory::getInstance()->distribute(instrumentID, TT_Open, direction, volume);
		if (vol > 0) {
			pc->onTradeDealt(instrumentID, TT_Open, price, volume, orderID, direction);
			volume -= vol;
		}		
	}

	open(orderID, instrumentID, direction, price, volume, ot);
	return orderID;
}

APORDERID APTrade::close(APASSETID instrumentID, APTradeDirection direction, double price, long volume, APPositionCtrl* pc, APOrderTimeCondition ot) {
	if (pc == NULL) {
		return INVALID_ORDER_ID;
	}

	APORDERID orderID = generateOrderID();
	APTradeOrderInfo info = { orderID, TT_Close, instrumentID, price, volume, direction, OS_None, "0", pc->getID() };
	m_localOrders[orderID] = info;

	if (APGlobalConfig::getInstance()->useRepertory()) {
		long vol = APPositionRepertory::getInstance()->distribute(instrumentID, TT_Close, direction, volume);
		if (vol > 0) {
			pc->onTradeDealt(instrumentID, TT_Close, price, volume, orderID, direction);
			volume -= vol;
		}
	}
	
	close(orderID, instrumentID, direction, price, volume,  ot);
	return orderID;
}

APORDERID APTrade::open(APASSETID instrumentID, APTradeDirection direction, APOrderPriceType orderPriceType, double price, APPositionCtrl * pc, APOrderTimeCondition orderTimeCondition, std::string date, APOrderVolumeCondition orderVolumeCondition, long volume, long minVolume, APOrderContingentCondition orderContingentCondition, double stopPrice)
{
	if (pc == NULL) {
		return INVALID_ORDER_ID;
	}

	APORDERID orderID = generateOrderID();
	APTradeOrderInfo info = { orderID, TT_Open, instrumentID, price, volume, direction, OS_None, "0", pc->getID() };
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
		return INVALID_ORDER_ID;
	}

	APORDERID orderID = generateOrderID();
	APTradeOrderInfo info = { orderID, TT_Close, instrumentID, price, volume, direction, OS_None, "0", pc->getID() };
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
		if (orderID != INVALID_ORDER_ID) {
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

std::string APTrade::serialize()
{
	Json::Value v;
	Json::Value arr;
	v["LocalOrders"] = arr;
	std::map<APORDERID, APTradeOrderInfo>::iterator it;
	int index = 0;
	for (it = m_localOrders.begin(); it != m_localOrders.end(); it++) {
		APTradeOrderInfo& oi = it->second;
		APOrderRecordInfo ori = convertOrderInfo(oi);
		Json::Value rec;
		rec["Direction"] = (int)ori.direction;
		rec["LocalID"] = ori.localID;
		rec["InstrumentID"] = ori.instrumentID;
		rec["RecordTime"] = ori.recordTime;
		rec["State"] = (int)ori.state;
		rec["SysID"] = ori.sysID;
		rec["VolumeSurplus"] = ori.volumeSurplus;
		arr[index++] = rec;
	}
	return std::string();
}

void APTrade::deserialize(std::string str)
{
	APJsonReader jr;
	jr.initWithString(str);

	m_orderRecordInfo.clear();

	int count = jr.getArraySize("LocalOrders");
	for (int i = 0; i < count; i++) {
		APOrderRecordInfo ori;
		memset(&ori, 0, sizeof(ori));
		std::string ordStr = jr.getArrayObjValue("LocalOrders", i);
		Json::Reader recReader;
		
		Json::Value rec;
		recReader.parse(ordStr, rec);

		ori.direction = (APTradeDirection)rec["Direction"].asInt();
		ori.localID = rec["LocalID"].asInt();
		ori.instrumentID = rec["InstrumentID"].asString();
		ori.recordTime = rec["RecordTime"].asString();
		ori.state = (APOrderState)rec["State"].asInt();
		ori.sysID = rec["SysID"].asString();
		ori.volumeSurplus = rec["VolumeSurplus"].asInt();

		m_orderRecordInfo[ori.localID] = ori;
	}
}

std::string APTrade::generateRedisKey()
{
	std::string redisKey = APAccountInfo::getInstance()->getInterfaceType() + ":"
		+ APAccountInfo::getInstance()->getAccountID() + ":Trade:RecordOrders";

	return redisKey;
}

APOrderRecordInfo APTrade::convertOrderInfo(APTradeOrderInfo & info)
{
	APOrderRecordInfo record;
	memset(&record, 0, sizeof(record));

	record.direction = info.direction;
	record.instrumentID = info.instrumentID;
	record.localID = info.orderID;
	record.recordTime = APTimeUtility::getDateTime();
	record.state = info.state;
	record.sysID = info.sysID;
	record.volumeSurplus = info.volumeSurplus;
	
	return record;
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
