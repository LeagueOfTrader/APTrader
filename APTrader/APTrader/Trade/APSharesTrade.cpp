#include "APSharesTrade.h"

APSharesTrade::APSharesTrade()
{
}

APSharesTrade::~APSharesTrade()
{
}

void APSharesTrade::open(APASSETID commodityID, APTrendType trend, double price, long amount, APTradeOrderType ot)
{
	if (trend == TT_Long) {
		buy(commodityID, price, amount);
	}
}

void APSharesTrade::close(APASSETID commodityID, APTrendType trend, double price, long amount, APTradeOrderType ot)
{
	if (trend == TT_Long) {
		sell(commodityID, price, amount);
	}
}

void APSharesTrade::cancel(APORDERID orderID)
{
}

long APSharesTrade::buy(APASSETID commodityID, double price, long amount)
{
	return 0;
}

long APSharesTrade::sell(APASSETID commodityID, double price, long amount)
{
	return 0;
}

void APSharesTrade::buyOrder(APASSETID commodityID, double price, long amount)
{
}

void APSharesTrade::sellOrder(APASSETID commodityID, double price, long amount)
{
}

void APSharesTrade::cancelOrder(long orderID)
{
}

void APSharesTrade::onBought(UINT orderID, double price, long amount)
{
}

void APSharesTrade::onSold(UINT orderID, double price, long amount)
{
}

void APSharesTrade::onCancel(UINT orderID)
{
}

void APSharesTrade::cancelBuyOrderBeyondPrice(double price)
{
}

void APSharesTrade::cancelSellOrderUnderPrice(double price)
{
}
