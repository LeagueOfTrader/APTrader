#include "../APMarco.h"
#include "APFuturesMarketQuotations.h"

#ifdef USE_CTP
#include "../Impl/CTP/APFuturesCTPMDAgent.h"
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

void APFuturesMarketQuotations::unSubscribeInstrumentInfo(APASSETID instrumentID)
{
#ifdef USE_CTP
	APFuturesCTPMDAgent::getInstance()->unSubscribeInstrument(instrumentID);
#endif
}
