#include "APSingleQuotationDecision.h"
#include "../APCommodityQuotation.h"
#include "../APMarketQuotationsManager.h"


APSingleQuotationDecision::APSingleQuotationDecision()
{
	m_quotation = NULL;
}

APSingleQuotationDecision::APSingleQuotationDecision(APASSETID commodityID)
{
	m_quotation = APMarketQuotationsManager::getInstance()->subscribeCommodity(commodityID);
}

APSingleQuotationDecision::~APSingleQuotationDecision()
{
	m_quotation = NULL;
}

double APSingleQuotationDecision::getValueReference()
{
	if (m_quotation != NULL) {	
		return m_quotation->getCurPrice();
	}

	return 0.0;
}
