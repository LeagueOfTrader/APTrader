#pragma once
#include "../APTrade.h"
#include <vector>

class APSharesTrade : public APTrade
{
public:
	APSharesTrade();
	~APSharesTrade();

public:
	virtual void open(APORDERID orderID, APASSETID instrumentID, APTradeDirection direction, double price, long volume, APOrderTimeCondition ot = OTC_GoodForDay);
	virtual void close(APORDERID orderID, APASSETID instrumentID, APTradeDirection direction, double price, long volume, APOrderTimeCondition ot = OTC_GoodForDay);

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

//protected:
//	long buy(APASSETID instrumentID, double price, long volume);
//	long sell(APASSETID instrumentID, double price, long volume);
//
//	void buyOrder(APASSETID instrumentID, double price, long volume);
//	void sellOrder(APASSETID instrumentID, double price, long volume);
//	void cancelOrder(long orderID);
//
//	void onBought(UINT orderID, double price, long volume);
//	void onSold(UINT orderID, double price, long volume);
//	void onCancel(UINT orderID);
//
//	void cancelBuyOrderBeyondPrice(double price);
//	void cancelSellOrderUnderPrice(double price);

protected:
	std::vector<APTradeOrderInfo> m_buyOrders;
	std::vector<APTradeOrderInfo> m_sellOrders;
};

