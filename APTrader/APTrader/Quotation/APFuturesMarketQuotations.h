#pragma once
#include "../APTypes.h"
#include "../APMarketQuotations.h"
#include "../Common/Singleton.h"

class APFuturesMarketQuotations :
	public APMarketQuotations, public Singleton<APFuturesMarketQuotations>
{
public:
	APFuturesMarketQuotations();
	~APFuturesMarketQuotations();

protected:
	virtual bool subscribeInstrumentInfo(APASSETID instrumentID);
};

