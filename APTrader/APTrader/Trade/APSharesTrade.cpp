#include "APSharesTrade.h"

APSharesTrade::APSharesTrade()
{
}

APSharesTrade::~APSharesTrade()
{
}

void APSharesTrade::open(APORDERID orderID, APASSETID instrumentID, APTrendType trend, double price, long volume, APOrderPriceType orderPriceType, APOrderTimeCondition ot)
{
	if (trend == TT_Long) {
		//buy(instrumentID, price, volume);
	}
}

void APSharesTrade::close(APORDERID orderID, APASSETID instrumentID, APTrendType trend, double price, long volume, APOrderPriceType orderPriceType, APOrderTimeCondition ot)
{
	if (trend == TT_Long) {
		//sell(instrumentID, price, volume);
	}
}

void APSharesTrade::cancel(APSYSTEMID sysID)
{
}

//long APSharesTrade::buy(APASSETID instrumentID, double price, long volume)
//{
//	return 0;
//}
//
//long APSharesTrade::sell(APASSETID instrumentID, double price, long volume)
//{
//	return 0;
//}
//
//void APSharesTrade::buyOrder(APASSETID instrumentID, double price, long volume)
//{
//}
//
//void APSharesTrade::sellOrder(APASSETID instrumentID, double price, long volume)
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
