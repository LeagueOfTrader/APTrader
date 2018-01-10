#pragma once
#include "../APQuotationDecision.h"
#include <string>
#include "../APTypes.h"

class APInstrumentQuotation;

class APPriceMarginQuotationDecision :
	public APQuotationDecision
{
public:
	APPriceMarginQuotationDecision(APASSETID srcInstrumentID, APASSETID targetInstrumentID, double upper, double lower);
	~APPriceMarginQuotationDecision();

	virtual double getValueReference();

protected:
	APInstrumentQuotation* m_srcQuotation;
	APInstrumentQuotation* m_targetQuotation;

	double m_priceDiff;
};

