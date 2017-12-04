#include "APSharesTrade.h"

APSharesTrade::APSharesTrade()
{
}

APSharesTrade::~APSharesTrade()
{
}

void APSharesTrade::open(APORDERID orderID, APASSETID commodityID, APTrendType trend, double price, long volume, APTradeOrderType ot)
{
	if (trend == TT_Long) {
		//buy(commodityID, price, volume);
	}
}

void APSharesTrade::close(APORDERID orderID, APASSETID commodityID, APTrendType trend, double price, long volume, APTradeOrderType ot)
{
	if (trend == TT_Long) {
		//sell(commodityID, price, volume);
	}
}

void APSharesTrade::cancel(APORDERID orderID)
{
}

//long APSharesTrade::buy(APASSETID commodityID, double price, long volume)
//{
//	return 0;
//}
//
//long APSharesTrade::sell(APASSETID commodityID, double price, long volume)
//{
//	return 0;
//}
//
//void APSharesTrade::buyOrder(APASSETID commodityID, double price, long volume)
//{
//}
//
//void APSharesTrade::sellOrder(APASSETID commodityID, double price, long volume)
//{
//}
//
//void APSharesTrade::cancelOrder(long orderID)
//{
//}
//
//void APSharesTrade::onBought(UINT orderID, double price, long volume)
//{
//}
//
//void APSharesTrade::onSold(UINT orderID, double price, long volume)
//{
//}
//
//void APSharesTrade::onCancel(UINT orderID)
//{
//}
//
//void APSharesTrade::cancelBuyOrderBeyondPrice(double price)
//{
//}
//
//void APSharesTrade::cancelSellOrderUnderPrice(double price)
//{
//}
