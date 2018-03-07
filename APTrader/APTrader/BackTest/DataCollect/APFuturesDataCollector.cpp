#include "APFuturesDataCollector.h"



APFuturesDataCollector::APFuturesDataCollector()
{
}


APFuturesDataCollector::~APFuturesDataCollector()
{
}

#ifdef USE_CTP
void APFuturesDataCollector::collect(std::string instrumentID, CThostFtdcDepthMarketDataField & data)
{
	//
}
#endif
