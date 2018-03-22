#include "APMacro.h"
#include "APPositionCtrl.h"
#include "APTradeManager.h"
#include "APGlobalConfig.h"
#include "Utils/APJsonReader.h"
#include "APInstrumentQuotation.h"
#include "APMarketDataManager.h"
#include "APPositionManager.h"
#include "APTrade.h"
#include "APPositionRepertory.h"

#include "Utils/APLog.h"
#include "Utils/APTimeUtility.h"

#include "APPositionObserver.h"

APPositionCtrl::APPositionCtrl()
{
	m_quotation = NULL;
	m_tag = "";
	m_priority = 1;

	m_openOrdersPosition = 0;
	m_closeOrdersPosition = 0;

	m_holdPosition = 0;
	m_availablePosition = 0;
	m_maxPosition = 0;
	m_marginPosition = 0;
	m_frozenPosition = 0;

	m_todayHoldPosition = 0;
	m_yesterdayHoldPosition = 0;

	m_cancelOpenOrdersPosition = 0;
	m_cancelCloseOrdersPosition = 0;
}

APPositionCtrl::~APPositionCtrl()
{
	if (m_quotation != NULL) {
		APMarketDataMgr->unSubscribeInstrument(m_quotation->getInstrumentID());
		m_quotation = NULL;
	}
	//APTradeManager::getInstance()->unregisterPositionCtrl(this);
	APPositionManager::getInstance()->removePositionCtrl(this);
}

//void APPositionCtrl::subscribeGoodsInfo()
//{
//}

void APPositionCtrl::setTag(std::string tag) {
	m_tag = tag;
}

int APPositionCtrl::getPriority()
{
	return m_priority;
}

void APPositionCtrl::setInstrumentID(APASSETID instrumentID)
{
	m_instrumentID = instrumentID;
}

const APASSETID& APPositionCtrl::getInstrumentID()
{
	return m_instrumentID;
}

APTradeDirection APPositionCtrl::getTradeDirection() 
{
	return m_directionType;
}

void APPositionCtrl::setAvailableFund(double fund)
{
	m_availableFund = fund;
}

void APPositionCtrl::setFrozenFund(double fund)
{
	m_frozenFund = fund;
}

void APPositionCtrl::setFreeFund(double fund)
{
	m_freeFund = fund;
}

double APPositionCtrl::getAvailableFund()
{
	return m_availableFund;
}

double APPositionCtrl::getFreeFund()
{
	return m_freeFund;
}

double APPositionCtrl::getFrozenFund()
{
	return m_frozenFund;
}

void APPositionCtrl::setMaxPosition(long position)
{
	m_maxPosition = position;
}

void APPositionCtrl::setFrozenPosition(long position)
{
	m_frozenPosition = position;

	if (m_holdPosition > 0) {
		m_availablePosition = m_holdPosition - m_frozenPosition;
	}
}

void APPositionCtrl::setMarginPosition(long position)
{
	m_marginPosition = position;
}

void APPositionCtrl::setHoldPosition(long position)
{
	m_holdPosition = position;

	if (m_holdPosition > 0) {
		m_availablePosition = m_holdPosition - m_frozenPosition;
	}
}

void APPositionCtrl::resetAvailablePosition()
{
	m_marginPosition = m_maxPosition - m_holdPosition;
}

long APPositionCtrl::getMaxPosition()
{
	return m_maxPosition;
}

long APPositionCtrl::getMarginPosition()
{
	return m_marginPosition;
}

long APPositionCtrl::getFrozenPosition()
{
	return m_frozenPosition;
}

long APPositionCtrl::getForeseeableHoldPosition()
{
	m_positionMutex.lock();
	long amount = m_holdPosition + m_openOrdersPosition;
	m_positionMutex.unlock();
	return amount;
}

//void APPositionCtrl::unfreezePosition(long position)
//{
//	if (position == -1){
//		position = m_frozenPosition;
//	}
//	
//	m_availablePosition += position;
//	m_frozenPosition -= position;
//}

std::vector<APPositionData> APPositionCtrl::getHoldPositionDetail()
{
	std::vector<APPositionData> holdPosList;
	APPositionData pd;
	//memset(&pd, 0, sizeof(pd));
	pd.clear();
	pd.instrumentID = m_instrumentID;
	pd.direction = m_directionType;
	pd.holdPosition = m_holdPosition;
	if (m_directionType == TD_Sell) {
		pd.longFrozenPosition = m_closeOrdersPosition;
	}
	else if (m_directionType == TD_Buy) {
		pd.shortFrozenPosition = m_closeOrdersPosition;
	}
	holdPosList.push_back(pd);
	return holdPosList;
}

void APPositionCtrl::setHoldAmount(APASSETID instrumentID, long amount)
{
	if (m_instrumentID == instrumentID) {
		m_holdPosition = amount;
	}
}

void APPositionCtrl::recycleFund(APRecycleFundLevel level)
{
	switch (level) {
		case RFL_FreeFund:
		case RFL_UnsealedOrder:
		case RFL_ProfitPosition:
			break;
		case RFL_UndeficitPosition:
			break;
		default:
			break;
	}
}

long APPositionCtrl::getHoldPosition()
{
	return m_holdPosition;
}

long APPositionCtrl::getAvailablePosition()
{
	return m_availablePosition;
}

long APPositionCtrl::getOpenOrdersPosition()
{
	return m_openOrdersPosition;
}

long APPositionCtrl::getCloseOrdersPosition()
{
	return m_closeOrdersPosition;
}

void APPositionCtrl::setOpenOrdersPosition(long pos)
{
	m_openOrdersPosition = pos;
}

void APPositionCtrl::setCloseOrdersPosition(long pos)
{
	m_closeOrdersPosition = pos;
}

void APPositionCtrl::relateOrder(APORDERID orderID)
{
	if (m_trade != NULL) {
		APTradeOrderInfo info;
		bool ret = m_trade->getOrderInfo(orderID, info);
		if (ret) {
			if (info.type == TT_Open) {
				m_openOrderList.push_back(orderID);
			}
			else {
				m_closeOrderList.push_back(orderID);
			}

			modifyOrdersPosition(info);

			m_trade->bindOrder(orderID, this);
		}
	}
}

void APPositionCtrl::openPosition(long volume)
{
	double price = 0.0;
	if (getLimitPrice(m_instrumentID, TT_Open, price)) {
		openPosition(price, volume);
	}
}

void APPositionCtrl::closePosition(long volume)
{
	double price = 0.0;
	if (getLimitPrice(m_instrumentID, TT_Close, price)) {
		closePosition(price, volume);
	}
}

void APPositionCtrl::openFullPosition()
{
	double price = 0.0;
	if (getLimitPrice(m_instrumentID, TT_Close, price)) {
		openFullPosition(price);
	}
}

void APPositionCtrl::closeOffPosition()
{
	double price = 0.0;
	if (getLimitPrice(m_instrumentID, TT_Close, price)) {
		closeOffPosition(price);
	}
}

bool APPositionCtrl::getMarketPrice(APASSETID instrumentID, APTradeType tradeType, APTradeDirection direction, double& price)
{
	if (m_quotation != NULL) {
		if (tradeType != TT_Open) {
			direction = getReversedDirection(direction);
		}
		price = m_quotation->getOpponentPrice(direction);

		return true;
	}

	return false;
}

bool APPositionCtrl::getLimitPrice(APASSETID instrumentID, APTradeType tradeType, double & price)
{
	if (m_quotation != NULL) {
		price = 0.0;
		double preClosePrice = m_quotation->getPreClosePrice();
		if (tradeType == TT_Open) {			
			price = preClosePrice * 1.1;
		}
		else {
			price = preClosePrice * 0.9;
		}

		return true;
	}

	return false;
}

void APPositionCtrl::modifyOrdersPosition(const APTradeOrderInfo& info)
{
	APASSETID instrumentID = info.instrumentID;
	APTradeDirection direction = info.direction;
	APTradeType tradeType = info.type;
	long deltaVolume = info.volumeSurplus;

	if (m_instrumentID != instrumentID) {
		return;
	}

	if (m_directionType != direction) {
		return;
	}

	if (tradeType == TT_Open) {
		m_openOrdersPosition += deltaVolume;
	}
	else {
		m_closeOrdersPosition += deltaVolume;
	}
}

void APPositionCtrl::openPosition(double price, long volume)
{	
	//if (volume > m_marginPosition) {
	//	volume = m_marginPosition;
	//}

	if (volume <= 0) {
		return;
	}

	double quotePrice = price;
	
	if (m_quotation != NULL)
	{
		double marketPrice = m_quotation->getCurPrice();
		if (m_directionType == TD_Buy && price > marketPrice) {
			quotePrice = marketPrice;
		}
		else if (m_directionType == TD_Sell && marketPrice > price) {
			quotePrice = marketPrice;
		}
	}

	m_marginPosition -= volume;
	m_openOrdersPosition += volume;
	open(m_directionType, quotePrice, volume);
}

void APPositionCtrl::closePosition(double price, long volume)
{
	if (volume > m_availablePosition) {
		volume = m_availablePosition;
	}

	double quotePrice = price;
	APTradeDirection dir = getReversedDirection(m_directionType);

	if (m_quotation != NULL)
	{
		double marketPrice = m_quotation->getCurPrice();
		if (dir == TD_Buy && price > marketPrice) {
			quotePrice = marketPrice;
		}
		else if (dir == TD_Sell && marketPrice > price) {
			quotePrice = marketPrice;
		}
	}
	
	m_availablePosition -= volume;
	m_closeOrdersPosition += volume;
	
	close(dir, quotePrice, volume);
}

void APPositionCtrl::openFullPosition(double price)
{
	openAll(m_directionType, price);
	m_openOrdersPosition += m_marginPosition;
	m_marginPosition = 0;	
}

void APPositionCtrl::closeOffPosition(double price)
{
	APTradeDirection dir = getReversedDirection(m_directionType);
	closeAll(dir, price);
	m_closeOrdersPosition += m_availablePosition;
	m_availablePosition = 0;	
}

//void APPositionCtrl::cancelTrade(APTradeType type, double price, long volume)
//{
//	cancel(type, price, volume);
//}

void APPositionCtrl::cancelTrade(APTradeType type, double price, APTradeDirection direction, bool cascade)
{
	cancel(type, price, direction, cascade);
}

void APPositionCtrl::cancelTrade(APTradeType type, APTradeDirection direction)
{
}

void APPositionCtrl::cancelTrade(APTradeType type)
{
	cancel(type);
}

void APPositionCtrl::cancelAllTrade()
{
	cancelAll();
}

void APPositionCtrl::onTradeCanceled(APASSETID instrumentID, APTradeType type, double price, long volume, APORDERID orderID, APTradeDirection direction)
{
	onTradeRollback(instrumentID, type, volume, orderID, direction);

	if (m_positionObservers.size() > 0) {
		for (int i = 0; i < m_positionObservers.size(); i++) {
			m_positionObservers[i]->onTradeCanceled(instrumentID, type, price, volume, direction);
		}
	}
}

void APPositionCtrl::update()
{
}

void APPositionCtrl::bindTrade(APTrade * trade)
{
	m_trade = trade;
}

void APPositionCtrl::onTradeOrdered(APASSETID instrumentID, APTradeType type, double price, long volume, APORDERID orderID, APTradeDirection direction)
{
	if (m_positionObservers.size() > 0) {
		for (int i = 0; i < m_positionObservers.size(); i++) {
			m_positionObservers[i]->onTradeOrdered(instrumentID, type, price, volume, direction);
		}
	}
}

void APPositionCtrl::onTradeFinished(APASSETID instrumentID, APTradeType type, double price, long volume, APORDERID orderID, APTradeDirection direction)
{
	if (type == TT_Open) {
		m_openOrderList.remove(orderID);
	}
	else if (type == TT_Close) {
		m_closeOrderList.remove(orderID);
	}

	if (m_positionObservers.size() > 0) {
		for (int i = 0; i < m_positionObservers.size(); i++) {
			m_positionObservers[i]->onTradeFinished(instrumentID, type, price, volume, direction);
		}
	}
}

void APPositionCtrl::onTradeFailed(APASSETID instrumentID, APTradeType type, double price, long volume, APORDERID orderID, APTradeDirection direction)
{
	onTradeRollback(instrumentID, type, volume, orderID, direction);

	if (m_positionObservers.size() > 0) {
		for (int i = 0; i < m_positionObservers.size(); i++) {
			m_positionObservers[i]->onTradeFailed(instrumentID, type, price, volume, direction);
		}
	}
}

void APPositionCtrl::onOrderOutdated(APORDERID orderID)
{
	bool removed = false;
	std::list<APORDERID>::iterator it;
	for (it = m_openOrderList.begin(); it != m_openOrderList.end(); it++) {
		APORDERID& id = *it;
		if (id == orderID) {
			removed = true;
			m_openOrderList.erase(it);
			break;
		}
	}
	if (!removed) {
		for (it = m_closeOrderList.begin(); it != m_closeOrderList.end(); it++) {
			APORDERID& id = *it;
			if (id == orderID) {
				m_closeOrderList.erase(it);
				break;
			}
		}
	}
}

void APPositionCtrl::syncPosition()
{
	APAccountInfo::getInstance()->verifyPosition(m_instrumentID, m_directionType, this);
}

void APPositionCtrl::correctPosition()
{
	APPositionData & posData = APPositionRepertory::getInstance()->getPositionData(m_instrumentID, m_directionType);

	if (m_holdPosition > posData.holdPosition) {
		m_holdPosition = posData.holdPosition;
		posData.holdPosition = 0;
	}
	else {
		posData.holdPosition -= m_holdPosition;
	}
	m_availablePosition = m_holdPosition;	
	
	if (m_directionType == TD_Buy) {
		if (m_closeOrdersPosition > posData.longFrozenPosition) {
			m_closeOrdersPosition = posData.longFrozenPosition;
			posData.longFrozenPosition = 0;
		}
		else {
			posData.longFrozenPosition -= m_closeOrdersPosition;
		}
	}
	else if (m_directionType == TD_Sell) {
		if (m_closeOrdersPosition > posData.shortFrozenPosition) {
			m_closeOrdersPosition = posData.shortFrozenPosition;
			posData.shortFrozenPosition = 0;
		}
		else {
			posData.shortFrozenPosition -= m_closeOrdersPosition;
		}
	}
}

void APPositionCtrl::removeAllLocalOrders()
{
	m_openOrderList.clear();
	m_closeOrderList.clear();
}

void APPositionCtrl::forceClearOrdersPosition()
{
	m_openOrdersPosition = 0;

	m_availablePosition += m_closeOrdersPosition;
	m_closeOrdersPosition = 0;
}

void APPositionCtrl::outputInfo()
{
	APLogger->log("PositionCtrl: %d, hold: %d, frozen: %d, opening: %d, closing: %d. ", 
		getID(), m_holdPosition, m_frozenPosition, 
		m_openOrdersPosition, m_closeOrdersPosition);
}

void APPositionCtrl::onNewTransactionDay()
{
}

//void APPositionCtrl::onSyncPositionStatus(const APPositionData & pd)
//{
//	//m_marginPosition = m_maxPosition - pd.holdPosition;
//	//m_frozenPosition = pd.frozenPosition;
//	//m_availablePosition = pd.holdPosition;
//	//m_openOrdersPosition = pd.openOrdersPosition;
//	//m_closeOrdersPosition = pd.closeOrdersPosition;
//}
//
//void APPositionCtrl::syncPositionStatus()
//{
//	//APPositionData pd;
//	//if (APAccountInfo::getInstance()->getPositionData(m_instrumentID, m_directionType, pd)) {
//	//	onSyncPositionStatus(pd);
//	//}
//	//else {
//	//	m_marginPosition = m_maxPosition;
//	//	m_frozenPosition = 0;
//	//	m_availablePosition = 0;
//	//	m_openOrdersPosition = 0;
//	//	m_closeOrdersPosition = 0;
//	//}
//}



void APPositionCtrl::initWithData(std::string positionInfo)
{
	APJsonReader jr;
	jr.initWithString(positionInfo);
	m_instrumentID = jr.getStrValue("InstrumentID");
	m_maxPosition = jr.getIntValue("TotalPosition");
	std::string strDirection = jr.getStrValue("Direction");
	if (strDirection == "Buy") {
		m_directionType = TD_Buy;
	}
	else if (strDirection == "Sell") {
		m_directionType = TD_Sell;
	}

#ifndef SIM
	m_quotation = APMarketDataMgr->subscribeInstrument(m_instrumentID);
#else 
	m_quotation = NULL;
#endif // !_DEBUG
	
	// load & sync data
	if (APGlobalConfig::getInstance()->isManualPosition()) {
		return;
	}

	bool ret = load();
	if (ret) { 
		syncPosition(); 
	}

	if (m_trade != NULL) {
		std::list<APORDERID>::iterator it;
		for (it = m_openOrderList.begin(); it != m_openOrderList.end(); it++) {
			m_trade->bindOrder(*it, this);
		}
		for (it = m_closeOrderList.begin(); it != m_closeOrderList.end(); it++) {
			m_trade->bindOrder(*it, this);
		}
	}
}

std::string APPositionCtrl::serialize()
{
	Json::Value v = serializeToJsonValue();	

	Json::FastWriter fw;
	return fw.write(v);
}

void APPositionCtrl::deserialize(std::string str)
{
	APJsonReader jr;
	jr.initWithString(str);

	m_openOrdersPosition = jr.getIntValue("OpenOrdersPosition");
	m_closeOrdersPosition = jr.getIntValue("ClsoeOrdersPosition");
	m_holdPosition = jr.getIntValue("HoldPosition");
	m_availablePosition = jr.getIntValue("AvailablePosition");
	m_marginPosition = jr.getIntValue("MarginPosition");
	m_frozenPosition = jr.getIntValue("FrozenPosition");

	int count = jr.getArraySize("BidOrders");
	for (int i = 0; i < count; i++) {
		APORDERID orderID = jr.getArrayStrValue("BidOrders", 0);
		m_openOrderList.push_back(orderID);
	}

	count = jr.getArraySize("AskOrders");
	for (int i = 0; i < count; i++) {
		APORDERID orderID = jr.getArrayStrValue("AskOrders", 0);
		m_closeOrderList.push_back(orderID);
	}

	std::string dateTime = jr.getStrValue("DateTime");
	//todo: if beyond last transaction day, change yd hold and td hold
	//
}

std::string APPositionCtrl::generateRedisKey()
{
	std::string redisKey = APAccountInfo::getInstance()->getInterfaceType() + ":"
		+ APAccountInfo::getInstance()->getAccountID() + ":Position:"
		+ m_tag;
	return redisKey;
}

Json::Value APPositionCtrl::serializeToJsonValue()
{
	Json::Value v;
	v["OpenOrdersPosition"] = m_openOrdersPosition;
	v["CloseOrdersPosition"] = m_closeOrdersPosition;
	v["HoldPosition"] = m_holdPosition;
	v["AvailablePosition"] = m_availablePosition;
	v["MarginPosition"] = m_marginPosition;
	v["FrozenPosition"] = m_frozenPosition;
	v["DateTime"] = APTimeUtility::getDateTime();

	// array process
	Json::Value bidArr;
	std::list<APORDERID>::iterator it;
	int index = 0;
	for (it = m_openOrderList.begin(); it != m_openOrderList.end(); it++) {
		APORDERID orderID = *it;
		//char strOrderID[64];
		//sprintf(strOrderID, "%I64d", orderID);
		bidArr[index++] = orderID;
	}
	v["BidOrders"] = bidArr;
	Json::Value askArr;
	index = 0;
	for (it = m_closeOrderList.begin(); it != m_closeOrderList.end(); it++) {
		APORDERID orderID = *it;
		//char strOrderID[64];
		//sprintf(strOrderID, "%I64d", orderID);
		askArr[index++] = orderID;
	}
	v["AskOrders"] = askArr;

	return v;
}

//APTradeDirection APPositionCtrl::getReversedDirection(APTradeDirection direction)
//{
//	APTradeDirection rdir;
//	if (direction == TD_Buy) {
//		rdir = TD_Sell;
//	}
//	else {
//		rdir = TD_Buy;
//	}
//	return rdir;
//}

void APPositionCtrl::addObserver(APPositionObserver * observer)
{
	m_positionObservers.push_back(observer);
}

void APPositionCtrl::cancel(APTradeType type, double price, APTradeDirection direction, bool cascade)
{
}

void APPositionCtrl::cancelAll()
{
	if (m_trade != NULL) {
		std::list<APORDERID>::iterator it;
		for (it = m_openOrderList.begin(); it != m_openOrderList.end(); it++) {
			m_trade->cancel(*it, this);
		}
		for (it = m_closeOrderList.begin(); it != m_closeOrderList.end(); it++) {
			m_trade->cancel(*it, this);
		}
	}
}

void APPositionCtrl::cancel(APORDERID orderID)
{
	if (m_trade != NULL) {
		m_trade->cancel(orderID, this);
	}
}
