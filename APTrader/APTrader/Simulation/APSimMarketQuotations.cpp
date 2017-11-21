#include "APSimMarketQuotations.h"
#include "APSimFuturesQuotation.h"


APSimMarketQuotations::APSimMarketQuotations()
{
}


APSimMarketQuotations::~APSimMarketQuotations()
{
}

APCommodityQuotation * APSimMarketQuotations::generateQuotation(APASSETID commodityID)
{
	return new APSimFuturesQuotation(commodityID);
}
