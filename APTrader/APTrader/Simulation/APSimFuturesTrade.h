#pragma once
#include "../Trade/APFuturesTrade.h"
//#include "../Common/Cyclebuffer.h"

class APSimFuturesTrade :
	public APFuturesTrade
{
public:
	APSimFuturesTrade();
	~APSimFuturesTrade();

	virtual void open(APORDERID orderID, APASSETID commodityID, APTrendType trend, double price, long volume, APTradeOrderType ot = TOT_ValidTheDay);
	virtual void close(APORDERID orderID, APASSETID commodityID, APTrendType trend, double price, long volume, APTradeOrderType ot = TOT_ValidTheDay);
	virtual void cancel(APSYSTEMID sysID);

	virtual void onFundChanged(APASSETID commodityID, APTradeType type, double variableFund, APORDERID orderID, APTrendType trend = TT_Long);

//	virtual void cancel(APASSETID commodityID, APTradeType type, APTrendType trend, double price, long volume);
//
//	virtual void onTradeDealt(APASSETID commodityID, APTradeType type, double price, long volume, APORDERID orderID, APTrendType trend = TT_Long);
//
//private:
//	int findOrder(APASSETID commodityID, APTradeType type, double price, APTrendType trend);
//	int findOrder(APASSETID commodityID, APTradeType type, double price, long volume, APTrendType trend);

protected:

	//CycleBuffer<APSimTradeOrder>* m_orderRecord;
	std::map<int, APTradeOrderInfo> m_orderRecord;
};

