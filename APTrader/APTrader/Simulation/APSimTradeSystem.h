#pragma once
#include "../Common/Singleton.h"
#include <string>
#include <list>
#include "../APDef.h"
#include <map>
#include "APSimFuturesTrade.h"
#include <deque>

class APIntAccumulator;
class APSimFuturesQuotation;

struct APSimTradeStub {
	UINT tradeID;
	APASSETID commodityID;
	APTrendType trend;
	double costPrice;
	long amount;
	UINT orderID;
};

class APSimTradeSystem : public Singleton<APSimTradeSystem>
{
public:
	APSimTradeSystem();
	~APSimTradeSystem();

	UINT requestOpen(APASSETID commodityID, APTrendType trend, double price, long amount);
	UINT requestClose(APASSETID commodityID, APTrendType trend, double price, long amount);
	void requestCancel(UINT orderID);

	void init();
	void update();

	double calcFloatingProfit();

protected:
	void onTradeFinished(APORDERID orderID, APASSETID commodityID, APTradeType type, double price, long amount, APTrendType trend);
	bool arrangeTrade(APTradeOrderInfo& order);
	void closeTheDeal(APORDERID orderID, APASSETID commodityID, APTradeType type, double price, long amount, APTrendType trend);
	double calcProfitAndLoss(double costPrice, double currentPrice, long amount, APTrendType trend);

private:
	void validQuotation(APASSETID commodityID);
	double calcFloatingProfit(APSimTradeStub& tradeStub);

protected:
	APIntAccumulator* m_orderIDAccumulator;
	APIntAccumulator* m_tradeIDAccumulator;
	std::list<APTradeOrderInfo> m_orderList;
	std::map<APASSETID, APSimFuturesQuotation*> m_quotations;
	APSimFuturesTrade* m_trade;
	//std::queue<APSimTradeStub> m_tradeStubs;
	std::map<APASSETID, std::deque<APSimTradeStub>> m_tradeLongStubs;
	std::map<APASSETID, std::deque<APSimTradeStub>> m_tradeShortStubs;
};

