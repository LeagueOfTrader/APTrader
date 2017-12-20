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

	virtual void open(APTrendType type, double price, long volume);
	virtual void close(APTrendType type, double price, long volume);
	virtual void openAll(APTrendType type, double price);
	virtual void closeAll(APTrendType type, double price);

	//virtual void cancel(APTradeType type, double price, long volume);
	virtual void cancel(APTradeType type, double price, APTrendType trend = TT_Long);
	virtual void cancel(APTradeType type);
	//virtual void cancelAll();

	virtual void onTradeDealt(APASSETID instrumentID, APTradeType type,  double price, long deltaVolume, APORDERID orderID, APTrendType trend = TT_Long);
	virtual void onTradeCanceled(APASSETID instrumentID, APTradeType type, long volume, APORDERID orderID, APTrendType trend = TT_Long);

	void setContractID(APASSETID contractID);
	void setContractType(APTrendType type);

	// api trade futures directly
	void open(APASSETID contractID, APTrendType trend, double price, long volume, APOrderTimeCondition ot = OTC_GoodForDay);
	void close(APASSETID contractID, APTrendType trend, double price, long volume, APOrderTimeCondition ot = OTC_GoodForDay);



//protected:
	//std::map<APASSETID, APFutureContract> m_contracts;
};

