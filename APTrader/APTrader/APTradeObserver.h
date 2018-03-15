#pragma once
#include "Common/IdentifiedObject.h"
#include "APDef.h"

class APTradeObserver : public IdentifiedObject
{
public:

	virtual void onTradeDealt(APASSETID instrumentID, APTradeType type, double price, long deltaVolume, APORDERID orderID, APTradeDirection direction = TD_Buy) = 0;
	virtual void onTradeCanceled(APASSETID instrumentID, APTradeType type, double price, long volume, APORDERID orderID, APTradeDirection direction = TD_Buy) = 0;
	virtual void onTradeOrdered(APASSETID instrumentID, APTradeType type, double price, long volume, APORDERID orderID, APTradeDirection direction = TD_Buy) = 0;
	virtual void onTradeFinished(APASSETID instrumentID, APTradeType type, double price, long volume, APORDERID orderID, APTradeDirection direction = TD_Buy) = 0;
	virtual void onTradeFailed(APASSETID instrumentID, APTradeType type, double price, long volume, APORDERID orderID, APTradeDirection direction = TD_Buy) = 0;	
};

