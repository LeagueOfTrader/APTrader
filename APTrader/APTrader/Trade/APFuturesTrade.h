#pragma once
#include "../APTrade.h"

class APFuturesTrade :
	public APTrade
{
public:
	APFuturesTrade();
	~APFuturesTrade();

	virtual APORDERID close(APASSETID instrumentID, APTradeDirection direction, double price, long volume,
						APTradeObserver* ob, APOrderTimeCondition ot = OTC_GoodForDay);

	virtual void open(APORDERID orderID, APASSETID instrumentID, APTradeDirection direction, double price, long volume, 
						APOrderTimeCondition ot = OTC_GoodForDay);
	virtual long close(APORDERID orderID, APASSETID instrumentID, APTradeDirection direction, double price, long volume,
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

	virtual void init();

	static bool isInstrumentCloseTodayFirst(APASSETID instrumentID);

	virtual void onTraded(APASSETID instrumentID, APTradeType type, double price, long volume, APORDERID orderID, APTradeDirection direction = TD_Buy);
	virtual void onCanceled(APORDERID orderID);

protected:
	static std::set<std::string> m_closeTodayFirstList;

	std::set<APORDERID> m_closeTodayOrders;
};

