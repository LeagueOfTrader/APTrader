#include "APPriceMarginQuotationDecision.h"
#include "../APInstrumentQuotation.h"
#include "../APMarketDataManager.h"

APPriceMarginQuotationDecision::APPriceMarginQuotationDecision(APASSETID srcInstrumentID, APASSETID targetInstrumentID, 
																double upper, double lower) 
																: APQuotationDecision(upper, lower)
{
	m_srcQuotation = APMarketDataMgr->subscribeInstrument(srcInstrumentID);
	m_targetQuotation = APMarketDataMgr->subscribeInstrument(targetInstrumentID);
	m_priceDiff = DBL_MAX;
}


APPriceMarginQuotationDecision::~APPriceMarginQuotationDecision()
{
	if (m_srcQuotation != NULL) {
		APMarketDataMgr->unSubscribeInstrument(m_srcQuotation->getInstrumentID());
	}

	if (m_targetQuotation != NULL) {
		APMarketDataMgr->unSubscribeInstrument(m_targetQuotation->getInstrumentID());
	}
}

double APPriceMarginQuotationDecision::getValueReference()
{
	if (m_srcQuotation != NULL && m_targetQuotation != NULL
		&& m_srcQuotation->isValid() && m_targetQuotation->isValid()) {
		double srcPrice = m_srcQuotation->getCurPrice();
		double targetPrice = m_targetQuotation->getCurPrice();
		m_priceDiff = targetPrice - srcPrice;
	}

	return m_priceDiff;
}

