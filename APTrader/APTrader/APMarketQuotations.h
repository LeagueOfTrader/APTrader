#pragma once
#include <map>
#include <list>
#include <string>
#include "Common/InitializableObject.h"
#include "Common/Ticker.h"
#include "APDef.h"
#include "APTypes.h"

class APInstrumentQuotation;

class APMarketQuotations : public InitializableObject, public Ticker
{
public:
	APMarketQuotations();
	~APMarketQuotations();

	APInstrumentQuotation* subscribeInstrument(APASSETID instrumentID);
	void unSubscribeInstrument(APASSETID instrumentID);

	virtual void init();
	virtual void update();

protected:
	virtual APInstrumentQuotation* generateQuotation(APASSETID instrumentID);

	virtual bool subscribeInstrumentInfo(APASSETID instrumentID) = 0;
	virtual void unSubscribeInstrumentInfo(APASSETID instrumentID) = 0;

protected:
	std::map<APASSETID, APInstrumentQuotation*> m_instrumentQuotations;
	APFinancialInstrumentType m_marketType;
};

