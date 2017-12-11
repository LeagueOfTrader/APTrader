#pragma once
#include <string>
#include <vector>
#include "APTypes.h"

struct APOrderInfo {
	double price;
	long volume;
};

class APInstrumentQuotation
{
public:
	APInstrumentQuotation(APASSETID instrumentID);
	~APInstrumentQuotation();

	virtual void queryQuotation() = 0;
	
	virtual double getCurPrice();
	virtual double getOpenPrice(UINT index = 0);
	virtual double getClosePrice(UINT index = 0);
	virtual long getOpenVolume(UINT index = 0);
	virtual long getCloseVolume(UINT index = 0);

protected:
	APASSETID m_instrumentID;

	double m_curPrice;
	double m_openingPrice;
	double m_highestPriceDaily;
	double m_lowestPriceDaily;
	double m_hightestPriceHistory;
	double m_lowestPriceHistory;

	std::vector<APOrderInfo> m_openOrderList;
	std::vector<APOrderInfo> m_closeOrderList;
};

