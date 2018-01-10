#include "APPriceRatioQuotationDecision.h"
#include "../APInstrumentQuotation.h"
#include "../APMarketDataManager.h"

APPriceRatioQuotationDecision::APPriceRatioQuotationDecision(APASSETID srcInstrumentID, APASSETID targetInstrumentID, 
															double upper, double lower)
															: APQuotationDecision(upper, lower)
{
	m_srcQuotation = APMarketDataMgr->subscribeInstrument(srcInstrumentID);
	m_targetQuotation = APMarketDataMgr->subscribeInstrument(targetInstrumentID);
	m_priceRatio = -DBL_MAX;
}


APPriceRatioQuotationDecision::~APPriceRatioQuotationDecision()
{
	if (m_srcQuotation != NULL) {
		APMarketDataMgr->unSubscribeInstrument(m_srcQuotation->getInstrumentID());
		m_srcQuotation = NULL;
	}

	if (m_targetQuotation != NULL) {
		APMarketDataMgr->unSubscribeInstrument(m_targetQuotation->getInstrumentID());
		m_targetQuotation = NULL;
	}
}

double APPriceRatioQuotationDecision::getValueReference()
{
	if (m_srcQuotation != NULL && m_targetQuotation != NULL
		&& m_srcQuotation->isValid() && m_targetQuotation->isValid()) {
		double srcPrice = m_srcQuotation->getCurPrice();
		double targetPrice = m_targetQuotation->getCurPrice();
		m_priceRatio = targetPrice / srcPrice;
	}

	return m_priceRatio;
}
