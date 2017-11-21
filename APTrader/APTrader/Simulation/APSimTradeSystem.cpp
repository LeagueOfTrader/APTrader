#include "APSimTradeSystem.h"
#include "../Utils/APIntAccumulator.h";
#include "APSimFuturesQuotation.h"
#include "APSimMarketQuotations.h"
#include "../APTradeManager.h"
#include "../Utils/APLog.h"

APSimTradeSystem::APSimTradeSystem()
{
	m_orderIDAccumulator = new APIntAccumulator();
	m_tradeIDAccumulator = new APIntAccumulator();
}


APSimTradeSystem::~APSimTradeSystem()
{
	m_trade = NULL;
	delete m_orderIDAccumulator;
	m_orderIDAccumulator = NULL;
	delete m_tradeIDAccumulator;
	m_tradeIDAccumulator = NULL;
}

UINT APSimTradeSystem::requestOpen(APASSETID commodityID, APTrendType trend, double price, long amount)
{
	APORDERID orderID = m_orderIDAccumulator->generateID();
	APTradeOrderInfo order = {orderID, TDT_Open, commodityID, price, amount, trend};
	m_orderList.push_back(order);
	if (m_trade != NULL) {
		m_trade->onTradeOrdered(commodityID, TDT_Open, price, amount, orderID, trend);
	}
	return orderID;
}

UINT APSimTradeSystem::requestClose(APASSETID commodityID, APTrendType trend, double price, long amount)
{
	APORDERID orderID = m_orderIDAccumulator->generateID();
	APTradeOrderInfo order = { orderID, TDT_Close, commodityID, price, amount, trend };
	m_orderList.push_back(order);
	if (m_trade != NULL) {
		m_trade->onTradeOrdered(commodityID, TDT_Close, price, amount, orderID, trend);
	}
	return orderID;
}


void APSimTradeSystem::requestCancel(UINT orderID)
{
	std::list<APTradeOrderInfo>::iterator it = m_orderList.begin();
	while (it != m_orderList.end())
	{
		APTradeOrderInfo& order = *it;
		if (order.orderID == orderID) {
			APTradeType cancelType = TDT_Num;
			if (order.type == TDT_Open) {
				cancelType = TDT_CancelOpen;
			}
			else if (order.type == TDT_Close) {
				cancelType = TDT_CancelClose;
			}

			onTradeFinished(orderID, order.commodityID, cancelType, order.price, order.amount, order.trend);
			m_orderList.erase(it);
			break;
		}
		else {
			it++;
		}
	}
}

void APSimTradeSystem::init()
{
	m_trade = dynamic_cast<APSimFuturesTrade*>(APTradeManager::getInstance()->getTradeInstance());
}

void APSimTradeSystem::update()
{
	std::list<APTradeOrderInfo>::iterator it = m_orderList.begin();
	while(it != m_orderList.end())
	{
		if (arrangeTrade(*it)) {
			m_orderList.erase(it++);
		}
		else {
			it++;
		}
	}
}

double APSimTradeSystem::calcFloatingProfit()
{
	double floatingProfit = 0.0;
	std::map<std::string, std::deque<APSimTradeStub>>::iterator itor;

	for (itor = m_tradeLongStubs.begin(); itor != m_tradeLongStubs.end(); itor++) {
		std::deque<APSimTradeStub>& stubs = itor->second;
		std::deque<APSimTradeStub>::iterator it;
		for (it = stubs.begin(); it != stubs.end(); it++) {
			floatingProfit += calcFloatingProfit(*it);
		}
	}

	for (itor = m_tradeShortStubs.begin(); itor != m_tradeShortStubs.end(); itor++) {
		std::deque<APSimTradeStub>& stubs = itor->second;
		std::deque<APSimTradeStub>::iterator it;
		for (it = stubs.begin(); it != stubs.end(); it++) {
			floatingProfit += calcFloatingProfit(*it);
		}
	}

	return floatingProfit;
}

void APSimTradeSystem::onTradeFinished(APORDERID orderID, APASSETID commodityID, APTradeType type, double price, long amount, APTrendType trend)
{
	if (m_trade != NULL) {
		m_trade->onTradeFinished(commodityID, type, price, amount, orderID, trend);

		std::string tradeTypeStr;
		if (type == TDT_Open) {
			tradeTypeStr = "Open";
		}
		else if (type == TDT_Close) {
			tradeTypeStr = "Close";
		}
		APLogger->log("Sim Trade Finish, %s targetID: %s, contractType:%d, price:%f, amount:%d .", 
						tradeTypeStr.c_str(), commodityID.c_str(), trend, price, amount);
	}
}

bool APSimTradeSystem::arrangeTrade(APTradeOrderInfo & order)
{
	bool result = false;

	validQuotation(order.commodityID);
	double price = 0.0;
	APSimFuturesQuotation* quotation = m_quotations[order.commodityID];
	if (quotation != NULL) {
		double curPrice = quotation->getCurPrice();
		if (order.price >= curPrice) {
			if ((order.type == TDT_Open && order.trend == TT_Long) ||
				(order.type == TDT_Close && order.trend == TT_Short)) {
				result = true;
				price = curPrice;
			}
		}
		if (curPrice >= order.price) {
			if ((order.type == TDT_Open && order.trend == TT_Short) ||
				(order.type == TDT_Close && order.trend == TT_Long)) {
				result = true;
				price = curPrice;
			}
		}
	}

	if (result) {
		closeTheDeal(order.orderID, order.commodityID, order.type, price, order.amount, order.trend);
	}

	return result;
}

double APSimTradeSystem::calcProfitAndLoss(double costPrice, double currentPrice, long amount, APTrendType trend) {
	double profitAndLoss = 0.0;
	double unitPnL = 0.0;

	if (trend == TT_Long) {
		unitPnL = currentPrice - costPrice;
	}
	else if (trend == TT_Short) {
		unitPnL = costPrice - currentPrice;
	}

	profitAndLoss = (double)amount * unitPnL;

	return profitAndLoss;
}

void APSimTradeSystem::closeTheDeal(APORDERID orderID, APASSETID commodityID, APTradeType type, double price, long amount, APTrendType trend) {
	// process deal
	UINT tradeID = m_tradeIDAccumulator->generateID();
	if (type == TDT_Open) {
		APSimTradeStub stub = { tradeID, commodityID, trend, price, amount, orderID };
		if (trend == TT_Long) {
			m_tradeLongStubs[commodityID].push_back(stub);
		}
		else if (trend == TT_Short) {
			m_tradeShortStubs[commodityID].push_back(stub);
		}

		double account = 0.0;
		account = -price * (double)amount;
		if (m_trade != NULL) {
			m_trade->onFundChanged(commodityID, TDT_Open, account, orderID, trend);
		}
	}
	else if (type == TDT_Close) {
		bool finishProcess = false;
		double account = 0.0;
		std::deque<APSimTradeStub> emptyQueue;
		std::deque<APSimTradeStub>& stubsQueue = emptyQueue;
		if (trend == TT_Long) {
			stubsQueue = m_tradeLongStubs[commodityID];
		}
		else if (trend == TT_Short) {
			stubsQueue = m_tradeShortStubs[commodityID];
		}

		while (!finishProcess) {
			APSimTradeStub& stub = stubsQueue.front();
			if (stub.amount < amount) {
				account += calcProfitAndLoss(stub.costPrice, price, stub.amount, trend);
				account += stub.costPrice * stub.amount;
			}
			else {
				stub.amount -= amount;
				account += calcProfitAndLoss(stub.costPrice, price, amount, trend);
				account += stub.costPrice * amount;
				finishProcess = true;
			}
		}
		

		if (m_trade != NULL) {
			m_trade->onFundChanged(commodityID, TDT_Close, account, orderID, trend);
		}
	}
	
	onTradeFinished(orderID, commodityID, type, price, amount, trend);
}

void APSimTradeSystem::validQuotation(APASSETID commodityID)
{
	std::map<APASSETID, APSimFuturesQuotation*>::iterator it = m_quotations.find(commodityID);
	if (it == m_quotations.end()) {
		APSimFuturesQuotation* quotation = dynamic_cast<APSimFuturesQuotation*>(APSimMarketQuotations::getInstance()->subscribeCommodity(commodityID));
		m_quotations[commodityID] = quotation;
	}
}

double APSimTradeSystem::calcFloatingProfit(APSimTradeStub& tradeStub)
{
	APASSETID targetID = tradeStub.commodityID;
	validQuotation(targetID);
	double curPrice = 0.0;
	if (m_quotations[targetID] != NULL) {
		curPrice = m_quotations[targetID]->getCurPrice();				
		double curPnL = calcProfitAndLoss(tradeStub.costPrice, curPrice, tradeStub.amount, tradeStub.trend);
		double cost = tradeStub.costPrice * tradeStub.amount;
		return curPnL + cost;
	}

	return 0.0;
}
