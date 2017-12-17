#pragma once
#include "../APTrade.h"

class APFuturesTrade :
	public APTrade
{
public:
	APFuturesTrade();
	~APFuturesTrade();

	virtual void open(APORDERID orderID, APASSETID instrumentID, APTrendType trend, double price, long volume, 
						APOrderPriceType orderPriceType = OPT_LimitPrice,  APOrderTimeCondition ot = OTC_GoodForDay);
	virtual void close(APORDERID orderID, APASSETID instrumentID, APTrendType trend, double price, long volume, 
						APOrderPriceType orderPriceType = OPT_LimitPrice, APOrderTimeCondition ot = OTC_GoodForDay);

	virtual void cancel(APSYSTEMID sysID);
};

