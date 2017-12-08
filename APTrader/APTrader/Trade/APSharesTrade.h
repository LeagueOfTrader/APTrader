#pragma once
#include "../APTrade.h"
#include <vector>

class APSharesTrade : public APTrade
{
public:
	APSharesTrade();
	~APSharesTrade();

public:
	virtual void open(APORDERID orderID, APASSETID commodityID, APTrendType trend, double price, long volume, APTradeOrderType ot = TOT_ValidTheDay);
	virtual void close(APORDERID orderID, APASSETID commodityID, APTrendType trend, double price, long volume, APTradeOrderType ot = TOT_ValidTheDay);

	virtual void cancel(APSYSTEMID sysID);

//protected:
//	long buy(APASSETID commodityID, double price, long volume);
//	long sell(APASSETID commodityID, double price, long volume);
//
//	void buyOrder(APASSETID commodityID, double price, long volume);
//	void sellOrder(APASSETID commodityID, double price, long volume);
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

