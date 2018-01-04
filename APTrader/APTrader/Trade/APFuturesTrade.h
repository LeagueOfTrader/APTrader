#pragma once
#include "../APTrade.h"

class APFuturesTrade :
	public APTrade
{
public:
	APFuturesTrade();
	~APFuturesTrade();

	virtual void open(APORDERID orderID, APASSETID instrumentID, APTradeDirection direction, double price, long volume, 
						APOrderTimeCondition ot = OTC_GoodForDay);
	virtual void close(APORDERID orderID, APASSETID instrumentID, APTradeDirection direction, double price, long volume, 
						APOrderTimeCondition ot = OTC_GoodForDay);

	virtual void open(APASSETID instrumentID, APORDERID localOrderID, APTradeDirection direction,
						APOrderPriceType orderPriceType, double price,
						APOrderTimeCondition orderTimeCondition = OTC_GoodForDay, std::string date = "",
						APOrderVolumeCondition orderVolumeCondition = OVC_Any, long volume = 0, long minVolume = 0,
						APOrderContingentCondition orderContingentCondition = OCC_Immediately, double stopPrice = 0.0);
	virtual void close(APASSETID instrumentID, APORDERID localOrderID, APTradeDirection direction,
						APOrderPriceType orderPriceType, double price,
						APOrderTimeCondition orderTimeCondition = OTC_GoodForDay, std::string date = "",
						APOrderVolumeCondition orderVolumeCondition = OVC_Any, long volume = 0, long minVolume = 0,
						APOrderContingentCondition orderContingentCondition = OCC_Immediately, double stopPrice = 0.0);

	virtual void cancel(APORDERID orderID);
};

