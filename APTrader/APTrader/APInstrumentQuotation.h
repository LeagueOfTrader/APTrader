#pragma once
#include <string>
#include <vector>
#include "APTypes.h"
#include "Model/APKLine.h"
#include "Common/RefCountObject.h"

#include "APStructs.h"

class APInstrumentQuotation : public RefCountObject
{
public:
	APInstrumentQuotation(APASSETID instrumentID);
	~APInstrumentQuotation();

	virtual void queryQuotation() = 0;
	
	double getCurPrice();
	double getPreClosePrice();
	APKLine& getKLineData();
	double getAveragePrice();
	APASSETID getInstrumentID();

	virtual double getOpenPrice(UINT index = 0);
	virtual double getClosePrice(UINT index = 0);
	virtual long getOpenVolume(UINT index = 0);
	virtual long getCloseVolume(UINT index = 0);

	bool isValid();

protected:
	APASSETID m_instrumentID;

	double m_curPrice;
	double m_preClosePrice;
	APKLine m_kLineData;
	double m_averagePrice;

	double m_hightestPriceHistory;
	double m_lowestPriceHistory;

	std::vector<APBidInfo> m_openOrderList;
	std::vector<APBidInfo> m_closeOrderList;

	bool m_valid;
};

