#include "APSharesTrade.h"

APSharesTrade::APSharesTrade()
{
}

APSharesTrade::~APSharesTrade()
{
}

void APSharesTrade::open(APORDERID orderID, APASSETID instrumentID, APTrendType trend, double price, long volume, APOrderTimeCondition ot)
{
	if (trend == TT_Long) {
		//buy(instrumentID, price, volume);
	}
}

void APSharesTrade::close(APORDERID orderID, APASSETID instrumentID, APTrendType trend, double price, long volume, APOrderTimeCondition ot)
{
	if (trend == TT_Long) {
		//sell(instrumentID, price, volume);
	}
}

void APSharesTrade::open(APASSETID instrumentID, APORDERID localOrderID, APTrendType trend, APOrderPriceType orderPriceType, double price, APOrderTimeCondition orderTimeCondition, std::string date, APOrderVolumeCondition orderVolumeCondition, long volume, long minVolume, APOrderContingentCondition orderContingentCondition, double stopPrice)
{
}

void APSharesTrade::close(APASSETID instrumentID, APORDERID localOrderID, APTrendType trend, APOrderPriceType orderPriceType, double price, APOrderTimeCondition orderTimeCondition, std::string date, APOrderVolumeCondition orderVolumeCondition, long volume, long minVolume, APOrderContingentCondition orderContingentCondition, double stopPrice)
{
}

void APSharesTrade::cancel(APORDERID orderID)
{
}
