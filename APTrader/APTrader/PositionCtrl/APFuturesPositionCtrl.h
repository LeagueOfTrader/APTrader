#pragma once
#include "../APPositionCtrl.h"
#include <map>
#include <string>
#include "../Trade/APFuturesTrade.h"

//struct APFutureContract {
//	APASSETID contractID;
//	APTrendType type;
//	long position;
//};

class APFuturesPositionCtrl :
	public APPositionCtrl
{
public:
	APFuturesPositionCtrl();
	~APFuturesPositionCtrl();

	virtual void init(std::string positionInfo);

	virtual void open(APTrendType type, double price, long amount);
	virtual void close(APTrendType type, double price, long amount);
	virtual void openAll(APTrendType type, double price);
	virtual void closeAll(APTrendType type, double price);

	virtual void cancel(APTradeType type, double price, long amount);
	virtual void cancel(APTradeType type);
	virtual void cancelAll();

	virtual void onTradeFinished(APASSETID commodityID, APTradeType type,  double price, long amount, APTrendType trend = TT_Long);

	void setContractID(APASSETID contractID);
	void setContractType(APTrendType type);

	// api trade futures directly
	void open(APASSETID contractID, APTrendType trend, double price, long amount, APTradeOrderType ot = TOT_ValidTheDay);
	void close(APASSETID contractID, APTrendType trend, double price, long amount, APTradeOrderType ot = TOT_ValidTheDay);



//protected:
	//std::map<APASSETID, APFutureContract> m_contracts;
};

