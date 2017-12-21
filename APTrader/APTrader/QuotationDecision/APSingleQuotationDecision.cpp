#include "APSingleQuotationDecision.h"
#include "../APInstrumentQuotation.h"
#include "../APMarketDataManager.h"


APSingleQuotationDecision::APSingleQuotationDecision()
{
	m_quotation = NULL;
}

APSingleQuotationDecision::APSingleQuotationDecision(APASSETID instrumentID)
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
	if (m_quotation != NULL) {	
		return m_quotation->getCurPrice();
	}

	return 0.0;
}
