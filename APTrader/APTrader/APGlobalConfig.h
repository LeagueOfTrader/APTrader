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

	APFinancialInstrumentType getInstrumentType();
	UINT getTransactionDays(); // t + n

	double getNearByPrice();
	UINT getVisibleOrdersCount();

	APOpenPositionLimit getOpenLimitType();
	bool isAutoCorrectPosition();
	bool useRepertory();
	bool counteractPosition();

private:
	APFinancialInstrumentType m_financialInstrumentType;
	double m_nearByPrice;
	UINT m_visibleOrdersCount;

	APOpenPositionLimit m_openLimit;
	bool m_autoCorrectPosition;
	bool m_useRepertory;
	bool m_counteractPosition;
};