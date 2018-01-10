#pragma once
#include "../APQuotationDecision.h"
#include <string>
#include "../APTypes.h"

class APInstrumentQuotation;

class APPriceRatioQuotationDecision :
	public APQuotationDecision
{
public:
	APPriceRatioQuotationDecision(APASSETID srcInstrumentID, APASSETID targetInstrumentID, double upper, double lower);
	~APPriceRatioQuotationDecision();

	virtual double getValueReference();

protected:
	APInstrumentQuotation* m_srcQuotation;
	APInstrumentQuotation* m_targetQuotation;

	double m_priceRatio;
};

