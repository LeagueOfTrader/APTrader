#pragma once
#include "../APStrategy.h"
#include <string>
#include "../Utils/APInterpolator.h"
#include "../Quotation/APFuturesQuotation.h"

//class APFuturesPosCtrlWithTransfer;

class APTransferPositionStrategy : public APStrategy
{
public:
	APTransferPositionStrategy();
	~APTransferPositionStrategy();

	static APStrategy* create();

	virtual void init(std::string strategyInfo);
	virtual void update();
	virtual void alert();

private:
	void initWithTransferInfo(std::string transferInfo);

	bool isCloseToDeliver();
	void transferContracts();

	bool canArbitage();
	double getCurPriceMargin();
	bool canTransferWithCurrentPrice();

	void onFinishTransfer();

private:
	//APASSETID m_srcContractID; //use m_commodityID
	APASSETID m_targetContractID;
	UINT m_criticalDays;
	UINT m_deadlineDays;
	double m_priceMargin;
	double m_priceMarginMax; // linear
	double m_safePrice;
	APInterpolationMethodType m_interpType;
	
	APFuturesQuotation* m_targetQuotation;
	APFuturesQuotation* m_curQuotation;

	APTrendType m_trendType;
};

