#pragma once
#include "../../Common/Singleton.h"
#include <string>
#include "../../APMacro.h"

#ifdef USE_CTP
#include "ThostFtdcUserApiStruct.h"
#endif

class APFuturesDataCollector : public Singleton<APFuturesDataCollector>
{
public:
	APFuturesDataCollector();
	~APFuturesDataCollector();

#ifdef USE_CTP
	void collect(std::string instrumentID, CThostFtdcDepthMarketDataField& data);
#endif
};

