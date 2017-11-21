#pragma once
#include "../APPositionCtrl.h"

class APSharesPositionCtrl :
	public APPositionCtrl
{
public:
	APSharesPositionCtrl();
	~APSharesPositionCtrl();

	virtual void init(std::string positionInfo);

	virtual void open(APTrendType type, double price, long amount);
	virtual void close(APTrendType type, double price, long amount);
	virtual void openAll(APTrendType type, double price);
	virtual void closeAll(APTrendType type, double price);
	virtual void cancel(APTradeType type, double price, long amount);
	virtual void cancel(APTradeType type);
	virtual void cancelAll();

	virtual void onTradeFinished(APASSETID commodityID, APTradeType type,  double price, long amount, APTrendType trend = TT_Long);

//protected:
//	void buy(double price, long amount);
//	void sell(double price, long amount);
//	void buyAtLeast(double price, long amount);
//
//	void overBuy(double price);
//	void overSell(double price);
//
//	void onBuyOrdered(double price, long amount);
//	void onBought(double price, long amount);
//	void onSold(double price, long amount);

protected:
	//
};

