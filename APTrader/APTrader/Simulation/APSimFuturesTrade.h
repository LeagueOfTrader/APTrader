#pragma once
#include "../Trade/APFuturesTrade.h"
//#include "../Common/Cyclebuffer.h"

class APSimFuturesTrade :
	public APFuturesTrade
{
public:
	APSimFuturesTrade();
	~APSimFuturesTrade();

	virtual void open(APORDERID orderID, APASSETID instrumentID, APTradeDirection direction, double price, long volume, APOrderTimeCondition ot = OTC_GoodForDay);
	virtual void close(APORDERID orderID, APASSETID instrumentID, APTradeDirection direction, double price, long volume, APOrderTimeCondition ot = OTC_GoodForDay);
	virtual void cancel(APORDERID orderID);

	virtual void onFundChanged(APASSETID instrumentID, APTradeType type, double variableFund, APORDERID orderID, APTradeDirection direction = TD_Buy);

//	virtual void cancel(APASSETID instrumentID, APTradeType type, APTradeDirection direction, double price, long volume);
//
//	virtual void onTradeDealt(APASSETID instrumentID, APTradeType type, double price, long volume, APORDERID orderID, APTradeDirection direction = TD_Buy);
//
//private:
//	int findOrder(APASSETID instrumentID, APTradeType type, double price, APTradeDirection direction);
//	int findOrder(APASSETID instrumentID, APTradeType type, double price, long volume, APTradeDirection direction);

protected:

	//CycleBuffer<APSimTradeOrder>* m_orderRecord;
	std::map<int, APTradeOrderInfo> m_orderRecord;
};

