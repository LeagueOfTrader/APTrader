#pragma once
#include "../APQuotationDecision.h"
#include <string>
#include "../APTypes.h"

class APCommodityQuotation;

class APSingleQuotationDecision :
	public APQuotationDecision
{
public:
	APSingleQuotationDecision();
	APSingleQuotationDecision(APASSETID commodityID);
	~APSingleQuotationDecision();

	virtual double getValueReference();

protected:
	APCommodityQuotation* m_quotation;
};

