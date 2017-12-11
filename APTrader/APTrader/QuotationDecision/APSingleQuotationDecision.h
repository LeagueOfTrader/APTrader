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
	APSingleQuotationDecision(APASSETID instrumentID);
	~APSingleQuotationDecision();

	virtual double getValueReference();

protected:
	APInstrumentQuotation* m_quotation;
};

