#include "APSimSingleQuotationDecision.h"
#include "APSimMarketQuotations.h"


APSimSingleQuotationDecision::APSimSingleQuotationDecision(APASSETID commodityID)
{
	m_quotation = APSimMarketQuotations::getInstance()->subscribeCommodity(commodityID);
}


APSimSingleQuotationDecision::~APSimSingleQuotationDecision()
{
}
