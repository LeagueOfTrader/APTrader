#pragma once
#include "Common/Singleton.h"
#include "Common/InitializableObject.h"
#include "APDef.h"
#include "APTypes.h"

class APGlobalConfig : public InitializableObject, public Singleton<APGlobalConfig> 
{
public:
	APGlobalConfig();
	~APGlobalConfig();

	virtual void init();

	APFinancialInstrumentType getInstrumentType();
	UINT getTransactionDays(); // t + n

	double getNearByPrice();
	UINT getVisibleOrdersCount();

	APOpenPositionLimit getOpenLimitType();
	bool isAutoCorrectPosition();
	bool useRepertory();
	bool counteractPosition();
	bool isManualPosition();
	bool isAutoRunStrategy();

private:
	APFinancialInstrumentType m_financialInstrumentType;
	double m_nearByPrice;
	UINT m_visibleOrdersCount;

	APOpenPositionLimit m_openLimit;
	bool m_autoCorrectPosition;
	bool m_useRepertory;
	bool m_counteractPosition;
	bool m_manualPosition;
	bool m_autoRunStrategy;
};