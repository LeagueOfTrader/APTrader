#include "APSimSingleQuotationDecision.h"
#include "APSimMarketQuotations.h"


APSimSingleQuotationDecision::APSimSingleQuotationDecision(APASSETID instrumentID)
{
	m_quotation = APSimMarketQuotations::getInstance()->subscribeInstrument(instrumentID);
}


APSimSingleQuotationDecision::~APSimSingleQuotationDecision()
{
}
