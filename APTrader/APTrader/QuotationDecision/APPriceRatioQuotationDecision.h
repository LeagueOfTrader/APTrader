#pragma once
#include "APDualQuotationDecision.h"
#include <string>
#include "../APTypes.h"

class APInstrumentQuotation;

class APPriceRatioQuotationDecision :
	public APDualQuotationDecision
{
public:
	APPriceRatioQuotationDecision(APASSETID srcInstrumentID, APASSETID targetInstrumentID, double upper, double lower);
	~APPriceRatioQuotationDecision();

	virtual double getValueReference();

protected:
	double m_priceRatio;
};

