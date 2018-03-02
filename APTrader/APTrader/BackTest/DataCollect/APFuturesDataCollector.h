#pragma once
#include "../../Common/Singleton.h"

class APFuturesDataCollector : public Singleton<APFuturesDataCollector>
{
public:
	APFuturesDataCollector();
	~APFuturesDataCollector();
};

