#include "../APMarco.h"
#include "APFuturesQuotation.h"

#ifdef USE_CTP
#include "../Impl/APFuturesCTPMDAgent.h"
#endif

APFuturesQuotation::APFuturesQuotation(APASSETID instrumentID) : APInstrumentQuotation(instrumentID)
{
}


APFuturesQuotation::~APFuturesQuotation()
{
}

void APFuturesQuotation::queryQuotation()
{
	APFuturesCTPMDAgent::getInstance();
}
