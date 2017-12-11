#pragma once
#include "../APPositionCtrl.h"

class APSharesPositionCtrl :
	public APPositionCtrl
{
public:
	APSharesPositionCtrl();
	~APSharesPositionCtrl();

	virtual void init(std::string positionInfo);

	virtual void open(APTrendType type, double price, long volume);
	virtual void close(APTrendType type, double price, long volume);
	virtual void openAll(APTrendType type, double price);
	virtual void closeAll(APTrendType type, double price);
	virtual void cancel(APTradeType type, double price, APTrendType trend = TT_Long);
	virtual void cancel(APTradeType type);
	//virtual void cancelAll();

	virtual void onTradeDealt(APASSETID instrumentID, APTradeType type,  double price, long deltaVolume, APORDERID orderID, APTrendType trend = TT_Long);

};

