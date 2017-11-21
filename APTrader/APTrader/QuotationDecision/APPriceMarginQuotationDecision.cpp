#include "APPriceMarginQuotationDecision.h"
#include "../APCommodityQuotation.h"
#include "../APMarketQuotationsManager.h"

APPriceMarginQuotationDecision::APPriceMarginQuotationDecision(APASSETID srcCommodityID, APASSETID targetCommodityID, double priceDiff)
{
	m_srcQuotation = APMarketQuotationsManager::getInstance()->subscribeCommodity(srcCommodityID);
	m_targetQuotation = APMarketQuotationsManager::getInstance()->subscribeCommodity(targetCommodityID);
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
