#pragma once
#include <map>
#include <list>
#include <string>
#include "Common/Singleton.h"
#include "APDef.h"
#include "APTypes.h"

class APCommodityQuotation;

class APMarketQuotations
{
public:
	APMarketQuotations();
	~APMarketQuotations();

	APCommodityQuotation* subscribeCommodity(APASSETID commodityID);

	void init();
	void update();

protected:
	virtual APCommodityQuotation* generateQuotation(APASSETID commodityID);

protected:
	std::map<APASSETID, APCommodityQuotation*> m_commodityQuotations;
	APFinancialCommodityType m_marketType;
};

