#pragma once
#include "Common/Singleton.h"
#include "APDef.h"
#include "APTypes.h"

class APGlobalConfig : public Singleton<APGlobalConfig> 
{
public:
	APGlobalConfig();
	~APGlobalConfig();

	void init();

	APFinancialCommodityType getCommodityType();
	UINT getTransactionDays(); // t + n

	double getNearByPrice();
	UINT getVisibleOrdersCount();

	APOpenPositionLimit getOpenLimitType();

private:
	APFinancialCommodityType m_financialCommodityType;
	double m_nearByPrice;
	UINT m_visibleOrdersCount;

	APOpenPositionLimit m_openLimit;
};