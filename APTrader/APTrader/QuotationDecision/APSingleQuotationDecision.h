#pragma once
#include "../APQuotationDecision.h"
#include <string>
#include "../APTypes.h"

class APInstrumentQuotation;

class APSingleQuotationDecision :
	public APQuotationDecision
{
public:
	APSingleQuotationDecision();
	APSingleQuotationDecision(APASSETID instrumentID, double upper, double lower);
	~APSingleQuotationDecision();

	virtual double getValueReference();

	virtual APInstrumentQuotation* getQuotationByIndex(int index = 0);

protected:
	APInstrumentQuotation* m_quotation;
};

