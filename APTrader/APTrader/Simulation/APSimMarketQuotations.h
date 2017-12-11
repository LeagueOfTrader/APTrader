#pragma once
#include "../APMarketQuotations.h"
#include "../APTypes.h"

class APSimMarketQuotations : public APMarketQuotations, public Singleton<APSimMarketQuotations>
{
public:
	APSimMarketQuotations();
	~APSimMarketQuotations();

protected:
	virtual APInstrumentQuotation* generateQuotation(APASSETID instrumentID);
};

