#pragma once
#include "APTypes.h"
#include "APDef.h"

class APPositionObserver
{
public:
	virtual void onTradeOrdered(APASSETID instrumentID, APTradeType type, double price, long volume, APTradeDirection direction) = 0;
	virtual void onTradeCanceled(APASSETID instrumentID, APTradeType type, double price, long volume, APTradeDirection direction) = 0;
	virtual void onTradeFinished(APASSETID instrumentID, APTradeType type, double price, long volume, APTradeDirection direction) = 0;
	virtual void onTradeFailed(APASSETID instrumentID, APTradeType type, double price, long volume, APTradeDirection direction) = 0;
};

