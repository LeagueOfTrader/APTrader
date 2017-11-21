#pragma once
#include "../APCommodityQuotation.h"
class APFuturesQuotation :
	public APCommodityQuotation
{
public:
	APFuturesQuotation(APASSETID commodityID);
	~APFuturesQuotation();

	virtual void queryQuotation();
};

