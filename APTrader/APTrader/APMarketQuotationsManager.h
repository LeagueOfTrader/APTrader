#pragma once
#include "APMarketQuotations.h"
#include "Common/Singleton.h"

class APMarketQuotationsManager :
	public APMarketQuotations, public Singleton<APMarketQuotationsManager>
{
public:
	APMarketQuotationsManager();
	~APMarketQuotationsManager();
};

