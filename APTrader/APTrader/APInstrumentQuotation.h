#pragma once
#include <string>
#include <vector>
#include "APTypes.h"
#include "Model/APKLine.h"
#include "Common/RefCountObject.h"
#include <mutex>

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

	virtual double getBuyPrice(UINT index = 0);
	virtual double getSellPrice(UINT index = 0);
	virtual long getBuyVolume(UINT index = 0);
	virtual long getSellVolume(UINT index = 0);

	double getOpponentPrice(APTradeDirection dir);

	bool isValid();

protected:
	APASSETID m_instrumentID;

	double m_curPrice;
	double m_preClosePrice;
	APKLine m_kLineData;
	double m_averagePrice;

	double m_hightestPriceHistory;
	double m_lowestPriceHistory;

	std::vector<APBidInfo> m_buyList;
	std::vector<APBidInfo> m_sellList;

	std::mutex m_mutex;
	bool m_valid;
};

