#include "APSingleQuotationDecision.h"
#include "../APInstrumentQuotation.h"
#include "../APMarketQuotationsManager.h"


APSingleQuotationDecision::APSingleQuotationDecision()
{
	m_quotation = NULL;
}

APSingleQuotationDecision::APSingleQuotationDecision(APASSETID instrumentID)
{
	m_quotation = APMarketQuotationsManager::getInstance()->subscribeInstrument(instrumentID);
}

APSingleQuotationDecision::~APSingleQuotationDecision()
{
	m_quotation = NULL;
}

double APSingleQuotationDecision::getValueReference()
{
	if (m_quotation != NULL) {	
		return m_quotation->getCurPrice();
	}

	return 0.0;
}
