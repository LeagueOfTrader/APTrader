#include "APSingleQuotationDecision.h"
#include "../APInstrumentQuotation.h"
#include "../APMarketDataManager.h"


APSingleQuotationDecision::APSingleQuotationDecision()
{
	m_quotation = NULL;
}

APSingleQuotationDecision::APSingleQuotationDecision(APASSETID instrumentID, double upper, double lower) : APQuotationDecision(upper, lower)
{
	m_quotation = APMarketDataMgr->subscribeInstrument(instrumentID);
}

APSingleQuotationDecision::~APSingleQuotationDecision()
{
	if (m_quotation != NULL) {
		APMarketDataMgr->unSubscribeInstrument(m_quotation->getInstrumentID());
		m_quotation = NULL;
	}
}

double APSingleQuotationDecision::getValueReference()
{
	if (m_quotation != NULL && m_quotation->isValid()) {	
		double price = m_quotation->getCurPrice();
		if (price > 0.0) {
			return price;
		}
	}

	return -DBL_MAX;
}
