#include "APMarco.h"
#include "APPositionCtrl.h"
#include "APTradeManager.h"
#include "APGlobalConfig.h"
#include "Utils/APJsonReader.h"
#include "APInstrumentQuotation.h"
#include "APMarketDataManager.h"
#include "APPositionManager.h"
#include "APTrade.h"
#include "3rdParty/jsoncpp/include/json/writer.h"

APPositionCtrl::APPositionCtrl()
{
	m_quotation = NULL;
	m_tag = "";
	m_priority = 1;
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

APTradeDirection APPositionCtrl::getTrendType() {
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

void APPositionCtrl::setAvailablePosition(long position)
{
	m_availablePosition = position;
}

void APPositionCtrl::resetAvailablePosition()
{
	m_availablePosition = m_maxPosition - m_holdPosition - m_frozenPosition - m_openOrdersPosition;
}

long APPositionCtrl::getMaxPosition()
{
	return m_maxPosition;
}

long APPositionCtrl::getAvailablePosition()
{
	return m_availablePosition;
}

long APPositionCtrl::getFrozenPosition()
{
	return m_frozenPosition;
}

void APPositionCtrl::unfreezePosition(long position)
{
	if (position == -1){
		position = m_frozenPosition;
	}
	
	m_holdPosition += position;
	m_frozenPosition -= position;
}

std::vector<APPositionData> APPositionCtrl::getHoldPositionDetail()
{
	std::vector<APPositionData> holdPosList;
	APPositionData pd;
	memset(&pd, 0, sizeof(pd));
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

long APPositionCtrl::getCurPosition()
{
	return m_holdPosition + m_frozenPosition;
}

long APPositionCtrl::getTradablePosition()
{
	return m_holdPosition;
}

long APPositionCtrl::getOpenOrderedPosition()
{
	return m_openOrdersPosition;
}

long APPositionCtrl::getCloseOrderedPosition()
{
	return m_closeOrdersPosition;
}

void APPositionCtrl::openPosition(APTradeDirection direction, double price, long volume)
{	
	if (volume > m_availablePosition) {
		volume = m_availablePosition;
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

	m_availablePosition -= volume;
	m_openOrdersPosition += volume;
	open(direction, quotePrice, volume);
}

void APPositionCtrl::closePosition(APTradeDirection direction, double price, long volume)
{
	if (volume > m_holdPosition) {
		volume = m_holdPosition;
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

	m_holdPosition -= volume;
	m_closeOrdersPosition += volume;
	close(direction, quotePrice, volume);
}

void APPositionCtrl::openFullPosition(APTradeDirection direction, double price)
{
	openAll(direction, price);
	m_openOrdersPosition += m_availablePosition;
	m_availablePosition = 0;	
}

void APPositionCtrl::closeOffPosition(APTradeDirection direction, double price)
{
	closeAll(direction, price);
	m_closeOrdersPosition += m_holdPosition;
	m_holdPosition = 0;	
}

//void APPositionCtrl::cancelTrade(APTradeType type, double price, long volume)
//{
//	cancel(type, price, volume);
//}

void APPositionCtrl::cancelTrade(APTradeType type, double price, APTradeDirection direction)
{
	cancel(type, price, direction);
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

void APPositionCtrl::onSyncPositionStatus(const APPositionData & pd)
{
	//m_availablePosition = m_maxPosition - pd.holdPosition;
	//m_frozenPosition = pd.frozenPosition;
	//m_holdPosition = pd.holdPosition;
	//m_openOrdersPosition = pd.openOrdersPosition;
	//m_closeOrdersPosition = pd.closeOrdersPosition;
}

void APPositionCtrl::syncPositionStatus()
{
	//APPositionData pd;
	//if (APAccountAssets::getInstance()->getPositionData(m_instrumentID, m_directionType, pd)) {
	//	onSyncPositionStatus(pd);
	//}
	//else {
	//	m_availablePosition = m_maxPosition;
	//	m_frozenPosition = 0;
	//	m_holdPosition = 0;
	//	m_openOrdersPosition = 0;
	//	m_closeOrdersPosition = 0;
	//}
}

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

	

	syncPositionStatus();
}

std::string APPositionCtrl::serialize()
{
	Json::Value v;
	v["OpenOrdersPosition"] = Json::Value(m_openOrdersPosition);
	v["CloseOrdersPosition"] = Json::Value(m_closeOrdersPosition);
	v["HoldPosition"] = Json::Value(m_holdPosition);
	v["AvailablePosition"] = Json::Value(m_availablePosition);
	v["FrozenPosition"] = Json::Value(m_frozenPosition);

	// array process

	Json::FastWriter fw;
	return fw.write(v);
}

void APPositionCtrl::deserialize(std::string str)
{
	//
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
