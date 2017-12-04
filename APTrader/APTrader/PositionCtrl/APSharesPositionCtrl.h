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

	virtual void onTradeFinished(APASSETID commodityID, APTradeType type,  double price, long volume, APORDERID orderID, APTrendType trend = TT_Long);

//protected:
//	void buy(double price, long volume);
//	void sell(double price, long volume);
//	void buyAtLeast(double price, long volume);
//
//	void overBuy(double price);
//	void overSell(double price);
//
//	void onBuyOrdered(double price, long volume);
//	void onBought(double price, long volume);
//	void onSold(double price, long volume);

protected:
	//
};

