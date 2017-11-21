#include "APPriceRatioQuotationDecision.h"
#include "../APCommodityQuotation.h"
#include "../APMarketQuotationsManager.h"

APPriceRatioQuotationDecision::APPriceRatioQuotationDecision(APASSETID srcCommodityID, APASSETID targetCommodityID, double priceRatio)
{
	m_srcQuotation = APMarketQuotationsManager::getInstance()->subscribeCommodity(srcCommodityID);
	m_targetQuotation = APMarketQuotationsManager::getInstance()->subscribeCommodity(targetCommodityID);
	m_priceRatio = priceRatio;
}


APPriceRatioQuotationDecision::~APPriceRatioQuotationDecision()
{
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
