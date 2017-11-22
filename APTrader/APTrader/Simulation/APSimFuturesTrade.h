#pragma once
#include "../Trade/APFuturesTrade.h"
//#include "../Common/Cyclebuffer.h"

class APSimFuturesTrade :
	public APFuturesTrade
{
public:
	APSimFuturesTrade();
	~APSimFuturesTrade();

	virtual void open(APASSETID commodityID, APTrendType trend, double price, long amount, APTradeOrderType ot = TOT_ValidTheDay);
	virtual void close(APASSETID commodityID, APTrendType trend, double price, long amount, APTradeOrderType ot = TOT_ValidTheDay);
	virtual void cancel(APORDERID orderID);

	virtual void onFundChanged(APASSETID commodityID, APTradeType type, double variableFund, APORDERID orderID, APTrendType trend = TT_Long);

//	virtual void cancel(APASSETID commodityID, APTradeType type, APTrendType trend, double price, long amount);
//
//	virtual void onTradeFinished(APASSETID commodityID, APTradeType type, double price, long amount, APORDERID orderID, APTrendType trend = TT_Long);
//
//private:
//	int findOrder(APASSETID commodityID, APTradeType type, double price, APTrendType trend);
//	int findOrder(APASSETID commodityID, APTradeType type, double price, long amount, APTrendType trend);

protected:

	//CycleBuffer<APSimTradeOrder>* m_orderRecord;
	std::map<int, APTradeOrderInfo> m_orderRecord;
};

