#pragma once
#include "../APQuotationDecision.h"
#include <string>
#include "../APTypes.h"

class APCommodityQuotation;

class APPriceRatioQuotationDecision :
	public APQuotationDecision
{
public:
	APPriceRatioQuotationDecision(APASSETID srcCommodityID, APASSETID targetCommodityID, double priceRatio);
	~APPriceRatioQuotationDecision();

	virtual double getValueReference();

protected:
	APCommodityQuotation* m_srcQuotation;
	APCommodityQuotation* m_targetQuotation;

	double m_priceRatio;
};

