#include "APPositionCtrl.h"
#include "APTradeManager.h"
#include "APGlobalConfig.h"
#include "Utils\APJsonReader.h"
#include "APCommodityQuotation.h"
#include "APMarketQuotationsManager.h"
#include "APPositionManager.h"
#include "APTrade.h"

APPositionCtrl::APPositionCtrl()
{
	m_quotation = NULL;
}


APPositionCtrl::~APPositionCtrl()
{
	m_quotation = NULL;
	//APTradeManager::getInstance()->unregisterPositionCtrl(this);
	APPositionManager::getInstance()->removePositionCtrl(this);
}

//void APPositionCtrl::subscribeGoodsInfo()
//{
//}


void APPositionCtrl::setCommodityID(APASSETID commodityID)
{
	m_commodityID = commodityID;
}

const APASSETID& APPositionCtrl::getCommodityID()
{
	return m_commodityID;
}

APTrendType APPositionCtrl::getTrendType() {
	return m_trendType;
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

void APPositionCtrl::openTrade(APTrendType type, double price, long volume)
{	
	if (volume > m_availablePosition) {
		volume = m_availablePosition;
	}

	double quotePrice = price;
	
	if (m_quotation != NULL)
	{
		double marketPrice = m_quotation->getCurPrice();
		if (type == TT_Long && price > marketPrice) {
			quotePrice = marketPrice;
		}
		else if (type == TT_Short && marketPrice > price) {
			quotePrice = marketPrice;
		}
	}

	m_availablePosition -= volume;
	m_openOrdersPosition += volume;
	open(type, quotePrice, volume);
}

void APPositionCtrl::closeTrade(APTrendType type, double price, long volume)
{
	if (volume > m_holdPosition) {
		volume = m_holdPosition;
	}

	double quotePrice = price;

	if (m_quotation != NULL)
	{
		double marketPrice = m_quotation->getCurPrice();
		if (type == TT_Long && price < marketPrice) {
			quotePrice = marketPrice;
		}
		else if (type == TT_Short && marketPrice > price) {
			quotePrice = marketPrice;
		}
	}

	m_holdPosition -= volume;
	m_closeOrdersPosition += volume;
	close(type, quotePrice, volume);
}

void APPositionCtrl::openAllTrade(APTrendType type, double price)
{
	openAll(type, price);
	m_openOrdersPosition += m_availablePosition;
	m_availablePosition = 0;	
}

void APPositionCtrl::closeAllTrade(APTrendType type, double price)
{
	closeAll(type, price);
	m_closeOrdersPosition += m_holdPosition;
	m_holdPosition = 0;	
}

//void APPositionCtrl::cancelTrade(APTradeType type, double price, long volume)
//{
//	cancel(type, price, volume);
//}

void APPositionCtrl::cancelTrade(APTradeType type, double price, APTrendType trend)
{
	cancel(type, price, trend);
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
	if (type == TDT_Open) {
		m_openOrderList.remove(orderID);
	}
	else if (type == TDT_Close) {
		m_closeOrderList.remove(orderID);
	}
}

void APPositionCtrl::onSyncPositionStatus(const APPositionData & pd)
{
	m_availablePosition = m_maxPosition - pd.holdPosition;
	m_frozenPosition = pd.frozenPosition;
	m_holdPosition = pd.holdPosition;
	m_openOrdersPosition = pd.openOrdersPosition;
	m_closeOrdersPosition = pd.closeOrdersPosition;
}

void APPositionCtrl::syncPositionStatus()
{
	APPositionData pd;
	if (APAccountAssets::getInstance()->getPositionData(m_commodityID, m_trendType, pd)) {
		onSyncPositionStatus(pd);
	}
	else {
		m_availablePosition = m_maxPosition;
		m_frozenPosition = 0;
		m_holdPosition = 0;
		m_openOrdersPosition = 0;
		m_closeOrdersPosition = 0;
	}
}

void APPositionCtrl::setBaseParam(std::string positionInfo)
{
	APJsonReader jr;
	jr.initWithString(positionInfo);
	m_commodityID = jr.getStrValue("CommodityID");
	m_maxPosition = jr.getIntValue("TotalPosition");
	std::string strTrend = jr.getStrValue("Trend");
	if (strTrend == "Long") {
		m_trendType = TT_Long;
	}
	else if (strTrend == "Short") {
		m_trendType = TT_Short;
	}

#ifndef _DEBUG
	m_quotation = APMarketQuotationsManager::getInstance()->subscribeCommodity(m_commodityID);
#else 
	m_quotation = NULL;
#endif // !_DEBUG

	

	syncPositionStatus();
}

void APPositionCtrl::cancel(APTradeType type, double price, APTrendType trend)
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
