#pragma once
#include "../APQuotationDecision.h"
#include <string>
#include "../APTypes.h"

class APCommodityQuotation;

class APPriceMarginQuotationDecision :
	public APQuotationDecision
{
public:
	APPriceMarginQuotationDecision(APASSETID srcCommodityID, APASSETID targetCommodityID, double priceDiff);
	~APPriceMarginQuotationDecision();

	virtual double getValueReference();

protected:
	APCommodityQuotation* m_srcQuotation;
	APCommodityQuotation* m_targetQuotation;

	double m_priceDiff;
};

