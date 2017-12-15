#include "../APMarco.h"
#include "APFuturesMarketQuotations.h"

#ifdef USE_CTP
#include "../Impl/APFuturesCTPMDAgent.h"
#endif

APFuturesMarketQuotations::APFuturesMarketQuotations()
{
}


APFuturesMarketQuotations::~APFuturesMarketQuotations()
{
}

bool APFuturesMarketQuotations::subscribeInstrumentInfo(APASSETID instrumentID)
{
#ifdef USE_CTP
	return APFuturesCTPMDAgent::getInstance()->subscribeInstrument(instrumentID);
#else
	return true;
#endif
}