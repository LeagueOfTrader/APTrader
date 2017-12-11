#pragma once
#include <map>
#include <list>
#include <string>
#include "Common/Singleton.h"
#include "APDef.h"
#include "APTypes.h"

class APInstrumentQuotation;

class APMarketQuotations
{
public:
	APMarketQuotations();
	~APMarketQuotations();

	APInstrumentQuotation* subscribeInstrument(APASSETID instrumentID);

	void init();
	void update();

protected:
	virtual APInstrumentQuotation* generateQuotation(APASSETID instrumentID);

protected:
	std::map<APASSETID, APInstrumentQuotation*> m_instrumentQuotations;
	APFinancialInstrumentType m_marketType;
};

