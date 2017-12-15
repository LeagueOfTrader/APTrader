#pragma once
#include "APMarco.h"

#ifdef USE_CTP
	#include "Quotation/APFuturesMarketQuotations.h"
	#define APMarketDataMgr APFuturesMarketQuotations::getInstance()
#else
#endif // USE_CTP
