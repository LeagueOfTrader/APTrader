#pragma once
#include "APFuturesCombinationPosCtrl.h"

class APFuturesSyncCombPosCtrl :
	public APFuturesCombinationPosCtrl
{
public:
	APFuturesSyncCombPosCtrl();
	~APFuturesSyncCombPosCtrl();

	virtual void openPosition(long volume); // ignore price in this class
	virtual void closePosition(long volume);
	//virtual void openFullPosition();
	//virtual void closeOffPosition();
	virtual void setHoldPosition(long position);

	virtual void cancel(APTradeType type);

	virtual void onTradeDealt(APASSETID instrumentID, APTradeType type, double price, long deltaVolume, APORDERID orderID, APTradeDirection direction = TD_Buy);
	virtual void onTradeCanceled(APASSETID instrumentID, APTradeType type, long volume, APORDERID orderID, APTradeDirection direction = TD_Buy);
};

