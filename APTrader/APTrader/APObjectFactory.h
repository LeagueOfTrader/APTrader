#pragma once
#include "APDef.h"
#include <string>
#include "APTypes.h"

class APCommodityQuotation;
class APPositionCtrl;
class APQuotationDecision;
class APTrade;

class APObjectFactory
{
public:
	APObjectFactory();
	~APObjectFactory();

	static APCommodityQuotation* newCommodityQuotation(APFinancialCommodityType type, APASSETID commodityID);
	static APPositionCtrl* newPositionCtrl(APFinancialCommodityType type);
	static APTrade* newTrade(APFinancialCommodityType type);
	static APQuotationDecision* newQuotationDecision(APQuotationDecisionType type, APASSETID srcID, APASSETID targetID = "", double param = 0.0f);
};

