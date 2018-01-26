#include "APPriceRatioQuotationDecision.h"
#include "../APInstrumentQuotation.h"
#include "../APMarketDataManager.h"

APPriceRatioQuotationDecision::APPriceRatioQuotationDecision(APASSETID srcInstrumentID, APASSETID targetInstrumentID, 
															double upper, double lower)
															: APDualQuotationDecision(srcInstrumentID, targetInstrumentID,
																upper, lower)
{
	m_priceRatio = -DBL_MAX;
}


APPriceRatioQuotationDecision::~APPriceRatioQuotationDecision()
{
}

double APPriceRatioQuotationDecision::getValueReference()
{
	if (m_srcQuotation != NULL && m_targetQuotation != NULL
		&& m_srcQuotation->isValid() && m_targetQuotation->isValid()) {
		double srcPrice = m_srcQuotation->getCurPrice();
		double targetPrice = m_targetQuotation->getCurPrice();
		if (srcPrice > 0.0 && targetPrice > 0.0) {
			m_priceRatio = targetPrice / srcPrice;
		}
	}

	return m_priceRatio;
}

