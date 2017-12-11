#pragma once
#include "APDef.h"
#include <string>
#include "APTypes.h"

class APInstrumentQuotation;
class APPositionCtrl;
class APQuotationDecision;
class APTrade;

class APObjectFactory
{
public:
	APObjectFactory();
	~APObjectFactory();

	static APInstrumentQuotation* newInstrumentQuotation(APFinancialInstrumentType type, APASSETID instrumentID);
	static APPositionCtrl* newPositionCtrl(APFinancialInstrumentType type);
	static APTrade* newTrade(APFinancialInstrumentType type);
	static APQuotationDecision* newQuotationDecision(APQuotationDecisionType type, APASSETID srcID, APASSETID targetID = "", double param = 0.0f);
};

