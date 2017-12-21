#include "APPriceRatioQuotationDecision.h"
#include "../APInstrumentQuotation.h"
#include "../APMarketDataManager.h"

APPriceRatioQuotationDecision::APPriceRatioQuotationDecision(APASSETID srcInstrumentID, APASSETID targetInstrumentID, double priceRatio)
{
	m_srcQuotation = APMarketDataMgr->subscribeInstrument(srcInstrumentID);
	m_targetQuotation = APMarketDataMgr->subscribeInstrument(targetInstrumentID);
	m_priceRatio = priceRatio;
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
	if (m_srcQuotation != NULL && m_targetQuotation != NULL) {
		double srcPrice = m_srcQuotation->getCurPrice();
		double targetPrice = m_targetQuotation->getCurPrice();
		return targetPrice / srcPrice;
	}

	return -DBL_MAX;
}
