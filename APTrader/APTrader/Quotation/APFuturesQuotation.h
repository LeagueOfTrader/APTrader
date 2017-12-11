#pragma once
#include "../APInstrumentQuotation.h"
class APFuturesQuotation :
	public APInstrumentQuotation
{
public:
	APFuturesQuotation(APASSETID instrumentID);
	~APFuturesQuotation();

	virtual void queryQuotation();
};

