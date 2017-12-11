#include "APPriceMarginQuotationDecision.h"
#include "../APInstrumentQuotation.h"
#include "../APMarketQuotationsManager.h"

APPriceMarginQuotationDecision::APPriceMarginQuotationDecision(APASSETID srcInstrumentID, APASSETID targetInstrumentID, double priceDiff)
{
	m_srcQuotation = APMarketQuotationsManager::getInstance()->subscribeInstrument(srcInstrumentID);
	m_targetQuotation = APMarketQuotationsManager::getInstance()->subscribeInstrument(targetInstrumentID);
	m_priceDiff = priceDiff;
}


APPriceMarginQuotationDecision::~APPriceMarginQuotationDecision()
{
}

double APPriceMarginQuotationDecision::getValueReference()
{
	if (m_srcQuotation != NULL && m_targetQuotation != NULL) {
		double srcPrice = m_srcQuotation->getCurPrice();
		double targetPrice = m_targetQuotation->getCurPrice();
		return targetPrice - srcPrice;
	}

	return -DBL_MAX;
}
