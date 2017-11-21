#pragma once
#include <string>
#include "../APCommodityQuotation.h"

class APSharesQuotation : public APCommodityQuotation
{
public:
	APSharesQuotation(APASSETID shareID);
	~APSharesQuotation();

	virtual void queryQuotation();

protected:
	void syncData(std::string data);
};

