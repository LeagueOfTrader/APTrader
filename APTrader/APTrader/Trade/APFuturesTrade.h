#pragma once
#include "../APTrade.h"

class APFuturesTrade :
	public APTrade
{
public:
	APFuturesTrade();
	~APFuturesTrade();

	virtual void open(APORDERID orderID, APASSETID instrumentID, APTrendType trend, double price, long volume, APTradeOrderType ot = TOT_GoodForDay);
	virtual void close(APORDERID orderID, APASSETID instrumentID, APTrendType trend, double price, long volume, APTradeOrderType ot = TOT_GoodForDay);

	virtual void cancel(APSYSTEMID sysID);
};

