#include "APDualQuotationDecision.h"
#include "../APInstrumentQuotation.h"
#include "../APMarketDataManager.h"

APDualQuotationDecision::APDualQuotationDecision(APASSETID srcInstrumentID, APASSETID targetInstrumentID,
	double upper, double lower)
	: APQuotationDecision(upper, lower)
{
	m_srcQuotation = APMarketDataMgr->subscribeInstrument(srcInstrumentID);
	m_targetQuotation = APMarketDataMgr->subscribeInstrument(targetInstrumentID);
}


APDualQuotationDecision::~APDualQuotationDecision()
{
	if (m_srcQuotation != NULL) {
		APMarketDataMgr->unSubscribeInstrument(m_srcQuotation->getInstrumentID());
	}

	if (m_targetQuotation != NULL) {
		APMarketDataMgr->unSubscribeInstrument(m_targetQuotation->getInstrumentID());
	}
}

APInstrumentQuotation * APDualQuotationDecision::getQuotationByIndex(int index)
{
	if (index == 0) {
		return m_srcQuotation;
	}

	if (index == 1) {
		return m_targetQuotation;
	}

	return NULL;
}

