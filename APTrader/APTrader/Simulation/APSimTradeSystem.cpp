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

APSYSTEMID APSimTradeSystem::requestOpen(APORDERID orderID, APASSETID instrumentID, APTradeDirection direction, double price, long volume)
{	
	char strID[32];
	sprintf(strID, "%d", m_orderIDAccumulator->generateID());
	APSYSTEMID sysID = strID;
	APTradeOrderInfo order = {orderID, TT_Open, instrumentID, price, volume, direction, TS_Ordered, sysID, 0};
	m_orderList.push_back(order);
	if (m_trade != NULL) {
		m_trade->onTradeOrdered(instrumentID, TT_Open, price, volume, orderID, TS_Ordered, sysID, direction);
	}
	return sysID;
}

APSYSTEMID APSimTradeSystem::requestClose(APORDERID orderID, APASSETID instrumentID, APTradeDirection direction, double price, long volume)
{
	char strID[32];
	sprintf(strID, "%d", m_orderIDAccumulator->generateID());
	APSYSTEMID sysID = strID;
	APTradeOrderInfo order = { orderID, TT_Close, instrumentID, price, volume, direction, TS_Ordered, sysID, 0 };
	m_orderList.push_back(order);
	if (m_trade != NULL) {
		m_trade->onTradeOrdered(instrumentID, TT_Close, price, volume, orderID, TS_Ordered, sysID, direction);
	}
	return sysID;
}


void APSimTradeSystem::requestCancel(APSYSTEMID sysID)
{
	std::list<APTradeOrderInfo>::iterator it = m_orderList.begin();
	while (it != m_orderList.end())
	{
		APTradeOrderInfo& order = *it;
		if (order.sysID == sysID) {

			onTradeCanceled(order.orderID, sysID);
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

void APSimTradeSystem::onTradeDealt(APORDERID orderID, APASSETID instrumentID, APTradeType type, double price, long volume, APSYSTEMID sysID, APTradeDirection direction)
{
	if (m_trade != NULL) {
		m_trade->onTradeDealt(instrumentID, type, price, volume, orderID, TS_Filled, sysID, direction);

		std::string tradeTypeStr;
		if (type == TT_Open) {
			tradeTypeStr = "Open";
		}
		else if (type == TT_Close) {
			tradeTypeStr = "Close";
		}
		APLogger->log("Sim Trade Finish, %s targetID: %s, contractType:%d, price:%f, volume:%d .", 
						tradeTypeStr.c_str(), instrumentID.c_str(), direction, price, volume);
	}
}

void APSimTradeSystem::onTradeCanceled(APORDERID orderID, APSYSTEMID sysID)
{
	if (m_trade != NULL) {
		m_trade->onTradeCanceled(orderID, sysID);
	}
}

bool APSimTradeSystem::arrangeTrade(APTradeOrderInfo & order)
{
	bool result = false;

	validQuotation(order.instrumentID);
	double price = 0.0;
	APSimFuturesQuotation* quotation = m_quotations[order.instrumentID];
	if (quotation != NULL) {
		double curPrice = quotation->getCurPrice();
		if (order.price >= curPrice) {
			if ((order.type == TT_Open && order.direction == TD_Buy) ||
				(order.type == TT_Close && order.direction == TD_Sell)) {
				result = true;
				price = curPrice;
			}
		}
		if (curPrice >= order.price) {
			if ((order.type == TT_Open && order.direction == TD_Sell) ||
				(order.type == TT_Close && order.direction == TD_Buy)) {
				result = true;
				price = curPrice;
			}
		}
	}

	if (result) {
		closeTheDeal(order.orderID, order.instrumentID, order.type, price, order.volume, order.sysID, order.direction);
	}

	return result;
}

double APSimTradeSystem::calcProfitAndLoss(double costPrice, double currentPrice, long volume, APTradeDirection direction) {
	double profitAndLoss = 0.0;
	double unitPnL = 0.0;

	if (direction == TD_Buy) {
		unitPnL = currentPrice - costPrice;
	}
	else if (direction == TD_Sell) {
		unitPnL = costPrice - currentPrice;
	}

	profitAndLoss = (double)volume * unitPnL;

	return profitAndLoss;
}

void APSimTradeSystem::closeTheDeal(APORDERID orderID, APASSETID instrumentID, APTradeType type, double price, long volume, APSYSTEMID sysID, APTradeDirection direction) {
	// process deal
	UINT tradeID = m_tradeIDAccumulator->generateID();
	if (type == TT_Open) {
		APSimTradeStub stub = { tradeID, instrumentID, direction, price, volume, orderID };
		if (direction == TD_Buy) {
			m_tradeLongStubs[instrumentID].push_back(stub);
		}
		else if (direction == TD_Sell) {
			m_tradeShortStubs[instrumentID].push_back(stub);
		}

		double account = 0.0;
		account = -price * (double)volume;
		if (m_trade != NULL) {
			m_trade->onFundChanged(instrumentID, TT_Open, account, orderID, direction);
		}
	}
	else if (type == TT_Close) {
		bool finishProcess = false;
		double account = 0.0;
		std::deque<APSimTradeStub> emptyQueue;
		std::deque<APSimTradeStub>& stubsQueue = emptyQueue;
		if (direction == TD_Buy) {
			stubsQueue = m_tradeLongStubs[instrumentID];
		}
		else if (direction == TD_Sell) {
			stubsQueue = m_tradeShortStubs[instrumentID];
		}

		while (!finishProcess) {
			APSimTradeStub& stub = stubsQueue.front();
			if (stub.volume < volume) {
				account += calcProfitAndLoss(stub.costPrice, price, stub.volume, direction);
				account += stub.costPrice * stub.volume;
			}
			else {
				stub.volume -= volume;
				account += calcProfitAndLoss(stub.costPrice, price, volume, direction);
				account += stub.costPrice * volume;
				finishProcess = true;
			}
		}
		

		if (m_trade != NULL) {
			m_trade->onFundChanged(instrumentID, TT_Close, account, orderID, direction);
		}
	}
	
	onTradeDealt(orderID, instrumentID, type, price, 
		0, //volume, 
		sysID, direction);
}

void APSimTradeSystem::validQuotation(APASSETID instrumentID)
{
	std::map<APASSETID, APSimFuturesQuotation*>::iterator it = m_quotations.find(instrumentID);
	if (it == m_quotations.end()) {
		APSimFuturesQuotation* quotation = dynamic_cast<APSimFuturesQuotation*>(APSimMarketQuotations::getInstance()->subscribeInstrument(instrumentID));
		m_quotations[instrumentID] = quotation;
	}
}

double APSimTradeSystem::calcFloatingProfit(APSimTradeStub& tradeStub)
{
	APASSETID targetID = tradeStub.instrumentID;
	validQuotation(targetID);
	double curPrice = 0.0;
	if (m_quotations[targetID] != NULL) {
		curPrice = m_quotations[targetID]->getCurPrice();				
		double curPnL = calcProfitAndLoss(tradeStub.costPrice, curPrice, tradeStub.volume, tradeStub.direction);
		double cost = tradeStub.costPrice * tradeStub.volume;
		return curPnL + cost;
	}

	return 0.0;
}
