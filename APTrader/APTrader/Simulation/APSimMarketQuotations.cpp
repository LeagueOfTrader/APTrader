#include "APSimMarketQuotations.h"
#include "APSimFuturesQuotation.h"


APSimMarketQuotations::APSimMarketQuotations()
{
}


APSimMarketQuotations::~APSimMarketQuotations()
{
}

APInstrumentQuotation * APSimMarketQuotations::generateQuotation(APASSETID instrumentID)
{
	return new APSimFuturesQuotation(instrumentID);
}

bool APSimMarketQuotations::subscribeInstrumentInfo(APASSETID instrumentID)
{
	return true;
}

void APSimMarketQuotations::unSubscribeInstrumentInfo(APASSETID instrumentID)
{
}
