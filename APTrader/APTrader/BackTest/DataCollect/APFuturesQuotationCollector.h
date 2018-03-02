#pragma once
#include "../../Quotation/APFuturesQuotation.h"

class APFuturesQuotationCollector : public APFuturesQuotation
{
public:
	APFuturesQuotationCollector(APASSETID instrumentID);
	~APFuturesQuotationCollector();

	virtual void queryQuotation();
};

