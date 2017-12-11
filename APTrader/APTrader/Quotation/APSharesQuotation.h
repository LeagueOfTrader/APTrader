#pragma once
#include <string>
#include "../APInstrumentQuotation.h"

class APSharesQuotation : public APInstrumentQuotation
{
public:
	APSharesQuotation(APASSETID shareID);
	~APSharesQuotation();

	virtual void queryQuotation();

protected:
	void syncData(std::string data);
};

