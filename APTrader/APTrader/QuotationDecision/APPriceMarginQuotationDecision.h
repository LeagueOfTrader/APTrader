#pragma once
#include "APDualQuotationDecision.h"
#include <string>
#include "../APTypes.h"

class APInstrumentQuotation;

class APPriceMarginQuotationDecision :
	public APDualQuotationDecision
{
public:
	APPriceMarginQuotationDecision(APASSETID srcInstrumentID, APASSETID targetInstrumentID, double upper, double lower);
	~APPriceMarginQuotationDecision();

	virtual double getValueReference();
protected:
	double m_priceDiff;
};

