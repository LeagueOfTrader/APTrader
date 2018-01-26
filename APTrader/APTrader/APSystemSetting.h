#pragma once
#include "Common/Singleton.h"
#include "Common/InitializableObject.h"
#include "Utils/APRedisSerializedObject.h"

class APSystemSetting : public InitializableObject, public APRedisSerializedObject, public Singleton<APSystemSetting>
{
public:
	APSystemSetting();
	~APSystemSetting();

	void init();

	long getStrategyUpdateInterval();
	long getInitializeStateWaitInterval();
	long getMarketDataQueryInterval();
	long getHoldPositionQueryInterval();

protected:
	virtual std::string serialize();
	virtual void deserialize(std::string str);

protected:
	long m_strategyUpdateInterval;
	long m_initializeStateWaitInterval;
	long m_marketDataQueryInterval;
	long m_holdPositionQueryInterval;
};

