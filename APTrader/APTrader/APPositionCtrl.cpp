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

#include "Utils/APTimeUtility.h"

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

UINT APPositionCtrl::getID()
{
	return m_id;
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
}

void APPositionCtrl::setMarginPosition(long position)
{
	m_marginPosition = position;
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
	memset(&pd, 0, sizeof(pd));
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

void APPositionCtrl::openPosition(APTradeDirection direction, long volume)
{
	double price = 0.0;
	if (getMarketPrice(m_instrumentID, TT_Open, direction, price)) {
		openPosition(direction, price, volume);
	}
}

void APPositionCtrl::closePosition(APTradeDirection direction, long volume)
{
	double price = 0.0;
	if (getMarketPrice(m_instrumentID, TT_Close, direction, price)) {
		closePosition(direction, price, volume);
	}
}

void APPositionCtrl::openFullPosition(APTradeDirection direction)
{
	double price = 0.0;
	if (getMarketPrice(m_instrumentID, TT_Close, direction, price)) {
		openFullPosition(direction, price);
	}
}

void APPositionCtrl::closeOffPosition(APTradeDirection direction)
{
	double price = 0.0;
	if (getMarketPrice(m_instrumentID, TT_Close, direction, price)) {
		closeOffPosition(direction, price);
	}
}

bool APPositionCtrl::getMarketPrice(APASSETID instrumentID, APTradeType tradeType, APTradeDirection direction, double& price)
{
	if (m_quotation != NULL) {
		price = 0.0;
		if (tradeType == TT_Open) {
			if (direction == TD_Buy) {
				price = m_quotation->getSellPrice();
			}
			else {
				price = m_quotation->getBuyPrice();
			}
		}
		else {
			if (direction == TD_Buy) {
				price = m_quotation->getBuyPrice();
			}
			else {
				price = m_quotation->getSellPrice();
			}
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

void APPositionCtrl::openPosition(APTradeDirection direction, double price, long volume)
{	
	if (volume > m_marginPosition) {
		volume = m_marginPosition;
	}

	double quotePrice = price;
	
	if (m_quotation != NULL)
	{
		double marketPrice = m_quotation->getCurPrice();
		if (direction == TD_Buy && price > marketPrice) {
			quotePrice = marketPrice;
		}
		else if (direction == TD_Sell && marketPrice > price) {
			quotePrice = marketPrice;
		}
	}

	m_marginPosition -= volume;
	m_openOrdersPosition += volume;
	open(direction, quotePrice, volume);
}

void APPositionCtrl::closePosition(APTradeDirection direction, double price, long volume)
{
	if (volume > m_availablePosition) {
		volume = m_availablePosition;
	}

	double quotePrice = price;

	if (m_quotation != NULL)
	{
		double marketPrice = m_quotation->getCurPrice();
		if (direction == TD_Buy && price < marketPrice) {
			quotePrice = marketPrice;
		}
		else if (direction == TD_Sell && marketPrice > price) {
			quotePrice = marketPrice;
		}
	}
	
	m_availablePosition -= volume;
	m_closeOrdersPosition += volume;
	close(direction, quotePrice, volume);
}

void APPositionCtrl::openFullPosition(APTradeDirection direction, double price)
{
	openAll(direction, price);
	m_openOrdersPosition += m_marginPosition;
	m_marginPosition = 0;	
}

void APPositionCtrl::closeOffPosition(APTradeDirection direction, double price)
{
	closeAll(direction, price);
	m_closeOrdersPosition += m_availablePosition;
	m_availablePosition = 0;	
}

//void APPositionCtrl::cancelTrade(APTradeType type, double price, long volume)
//{
//	cancel(type, price, volume);
//}

void APPositionCtrl::cancelTrade(APTradeType type, double price, APTradeDirection direction)
{
	cancel(type, price, direction);
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

void APPositionCtrl::update()
{
}

void APPositionCtrl::bindTrade(APTrade * trade)
{
	m_trade = trade;
}

void APPositionCtrl::onCompleteOrder(APORDERID orderID, APTradeType type)
{
	if (type == TT_Open) {
		m_openOrderList.remove(orderID);
	}
	else if (type == TT_Close) {
		m_closeOrderList.remove(orderID);
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
		APORDERID orderID = jr.getArrayIntValue("BidOrders", 0);
		m_openOrderList.push_back(orderID);
	}

	count = jr.getArraySize("AskOrders");
	for (int i = 0; i < count; i++) {
		APORDERID orderID = jr.getArrayIntValue("AskOrders", 0);
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
		bidArr[index++] = orderID;
	}
	v["BidOrders"] = bidArr;
	Json::Value askArr;
	index = 0;
	for (it = m_closeOrderList.begin(); it != m_closeOrderList.end(); it++) {
		APORDERID orderID = *it;
		askArr[index++] = orderID;
	}
	v["AskOrders"] = askArr;

	return v;
}

void APPositionCtrl::cancel(APTradeType type, double price, APTradeDirection direction)
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
