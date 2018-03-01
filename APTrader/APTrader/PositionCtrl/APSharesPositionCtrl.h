#pragma once
#include "../APPositionCtrl.h"

class APSharesPositionCtrl :
	public APPositionCtrl
{
public:
	APSharesPositionCtrl();
	~APSharesPositionCtrl();

	virtual void init(std::string positionInfo);

	virtual void open(APTradeDirection direction, double price, long volume);
	virtual void close(APTradeDirection direction, double price, long volume);
	virtual void openAll(APTradeDirection direction, double price);
	virtual void closeAll(APTradeDirection direction, double price);
	virtual void cancel(APTradeType type, double price, APTradeDirection direction = TD_Buy);
	virtual void cancel(APTradeType type);
	//virtual void cancelAll();

	virtual void onTradeDealt(APASSETID instrumentID, APTradeType type,  double price, long deltaVolume, APORDERID orderID, APTradeDirection direction = TD_Buy);
	virtual void onTradeRollback(APASSETID instrumentID, APTradeType type, long volume, APORDERID orderID, APTradeDirection direction = TD_Buy);

	virtual void onNewTransactionDay();
};

