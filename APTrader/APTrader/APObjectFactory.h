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

	static APInstrumentQuotation* newInstrumentQuotation(APFinancialInstrumentType marketType, APASSETID instrumentID);
	static APPositionCtrl* newPositionCtrl(APFinancialInstrumentType marketType);
	static APTrade* newTrade(APFinancialInstrumentType marketType);
	static APQuotationDecision* newQuotationDecision(APQuotationDecisionType type, APASSETID srcID, APASSETID targetID = "", double param = 0.0f);
};

