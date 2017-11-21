#pragma once
#include "../QuotationDecision/APSingleQuotationDecision.h"
#include <string>
#include "../APTypes.h"

class APSimSingleQuotationDecision : public APSingleQuotationDecision
{
public:
	APSimSingleQuotationDecision(APASSETID commodityID);
	~APSimSingleQuotationDecision();
};

