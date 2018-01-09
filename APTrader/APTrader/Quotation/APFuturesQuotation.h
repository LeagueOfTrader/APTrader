#pragma once
#include "../APInstrumentQuotation.h"
#include "../APMacro.h"

#ifdef USE_CTP
#include "ThostFtdcUserApiStruct.h"
#endif

class APFuturesQuotation :
	public APInstrumentQuotation
{
public:
	APFuturesQuotation(APASSETID instrumentID);
	~APFuturesQuotation();

	virtual void queryQuotation();

private:
#ifdef USE_CTP
	void parseCTPData(CThostFtdcDepthMarketDataField *data);
#endif

private:
#ifdef USE_CTP
	int m_timestamp;
#endif
};

