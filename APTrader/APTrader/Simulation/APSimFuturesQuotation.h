#pragma once
#include "../Quotation/APFuturesQuotation.h"
class APSimFuturesQuotation :
	public APFuturesQuotation
{
public:
	APSimFuturesQuotation(APASSETID instrumentID);
	~APSimFuturesQuotation();

	virtual void queryQuotation();

	int getSimPricesCount();

private:
	void init(APASSETID instrumentID);

protected:
	int m_curIndex;
	bool m_finished;
	bool m_loop;

	std::vector<double> m_simPrices;
};

