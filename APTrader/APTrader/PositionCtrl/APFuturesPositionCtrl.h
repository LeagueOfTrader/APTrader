#pragma once
#include "../APPositionCtrl.h"
#include <map>
#include <string>
#include "../Trade/APFuturesTrade.h"

//struct APFutureContract {
//	APASSETID instrumentID;
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

	virtual void cancel(APTradeType type, double price, APTrendType trend = TT_Long);
	virtual void cancel(APTradeType type);

	virtual void onTradeDealt(APASSETID instrumentID, APTradeType type,  double price, long deltaVolume, APORDERID orderID, APTrendType trend = TT_Long);
	virtual void onTradeCanceled(APASSETID instrumentID, APTradeType type, long volume, APORDERID orderID, APTrendType trend = TT_Long);

	void setInstrumentID(APASSETID instrumentID);
	void setContractType(APTrendType type);

	// open/close directly
	void open(APASSETID instrumentID, APTrendType trend, double price, long volume, APOrderTimeCondition ot = OTC_GoodForDay);
	void close(APASSETID instrumentID, APTrendType trend, double price, long volume, APOrderTimeCondition ot = OTC_GoodForDay);
	// advanced open/close
	void open(APASSETID instrumentID, APTrendType trend,
		APOrderPriceType orderPriceType, double price,
		APOrderTimeCondition orderTimeCondition = OTC_GoodForDay, std::string date = "",
		APOrderVolumeCondition orderVolumeCondition = OVC_Any, long volume = 0, long minVolume = 0,
		APOrderContingentCondition orderContingentCondition = OCC_Immediately, double stopPrice = 0.0);
	void close(APASSETID instrumentID, APTrendType trend,
		APOrderPriceType orderPriceType, double price,
		APOrderTimeCondition orderTimeCondition = OTC_GoodForDay, std::string date = "",
		APOrderVolumeCondition orderVolumeCondition = OVC_Any, long volume = 0, long minVolume = 0,
		APOrderContingentCondition orderContingentCondition = OCC_Immediately, double stopPrice = 0.0);
};

