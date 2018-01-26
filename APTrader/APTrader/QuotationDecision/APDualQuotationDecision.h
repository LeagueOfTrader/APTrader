#pragma once
#include "../APQuotationDecision.h"
#include <string>
#include "../APTypes.h"

class APInstrumentQuotation;

class APDualQuotationDecision :
	public APQuotationDecision
{
public:
	APDualQuotationDecision(APASSETID srcInstrumentID, APASSETID targetInstrumentID, double upper, double lower);
	~APDualQuotationDecision();

	virtual double getValueReference() = 0;
	virtual APInstrumentQuotation* getQuotationByIndex(int index = 0);

protected:
	APInstrumentQuotation * m_srcQuotation;
	APInstrumentQuotation* m_targetQuotation;
};