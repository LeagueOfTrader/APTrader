#include "APQuotationDecision.h"
#include <float.h>


APQuotationDecision::APQuotationDecision()
{
	m_upperLimit = DBL_MAX / 2.0;
	m_lowerLimit = -DBL_MAX / 2.0;
}

APQuotationDecision::APQuotationDecision(double upper, double lower)
{
	m_upperLimit = upper;
	m_lowerLimit = lower;
}


APQuotationDecision::~APQuotationDecision()
{
}

bool APQuotationDecision::isValueRefValid(double valRef)
{
	if (valRef > m_upperLimit) {
		return false;
	}

	if (valRef < m_lowerLimit) {
		return false;
	}

	return true;
}
