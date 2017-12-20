#pragma once
#include "../Trade/APFuturesTrade.h"
//#include "../Common/Cyclebuffer.h"

class APSimFuturesTrade :
	public APFuturesTrade
{
public:
	APSimFuturesTrade();
	~APSimFuturesTrade();

	virtual void open(APORDERID orderID, APASSETID instrumentID, APTrendType trend, double price, long volume, APOrderTimeCondition ot = OTC_GoodForDay);
	virtual void close(APORDERID orderID, APASSETID instrumentID, APTrendType trend, double price, long volume, APOrderTimeCondition ot = OTC_GoodForDay);
	virtual void cancel(APORDERID orderID);

	virtual void onFundChanged(APASSETID instrumentID, APTradeType type, double variableFund, APORDERID orderID, APTrendType trend = TT_Long);

//	virtual void cancel(APASSETID instrumentID, APTradeType type, APTrendType trend, double price, long volume);
//
//	virtual void onTradeDealt(APASSETID instrumentID, APTradeType type, double price, long volume, APORDERID orderID, APTrendType trend = TT_Long);
//
//private:
//	int findOrder(APASSETID instrumentID, APTradeType type, double price, APTrendType trend);
//	int findOrder(APASSETID instrumentID, APTradeType type, double price, long volume, APTrendType trend);

protected:

	//CycleBuffer<APSimTradeOrder>* m_orderRecord;
	std::map<int, APTradeOrderInfo> m_orderRecord;
};

