#pragma once
#include "../APTrade.h"

class APFuturesTrade :
	public APTrade
{
public:
	APFuturesTrade();
	~APFuturesTrade();

	virtual void open(APASSETID commodityID, APTrendType trend, double price, long amount, APTradeOrderType ot = TOT_ValidTheDay);
	virtual void close(APASSETID commodityID, APTrendType trend, double price, long amount, APTradeOrderType ot = TOT_ValidTheDay);

	virtual void cancel(APORDERID orderID);
};

