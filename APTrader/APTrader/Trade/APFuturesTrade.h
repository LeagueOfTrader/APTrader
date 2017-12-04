#pragma once
#include "../APTrade.h"

class APFuturesTrade :
	public APTrade
{
public:
	APFuturesTrade();
	~APFuturesTrade();

	virtual void open(APORDERID orderID, APASSETID commodityID, APTrendType trend, double price, long volume, APTradeOrderType ot = TOT_ValidTheDay);
	virtual void close(APORDERID orderID, APASSETID commodityID, APTrendType trend, double price, long volume, APTradeOrderType ot = TOT_ValidTheDay);

	virtual void cancel(APORDERID orderID);
};

