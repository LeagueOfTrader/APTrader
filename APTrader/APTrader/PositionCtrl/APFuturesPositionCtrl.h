#pragma once
#include "../APPositionCtrl.h"
#include <map>
#include <string>
#include "../Trade/APFuturesTrade.h"

//struct APFutureContract {
//	APASSETID instrumentID;
//	APTradeDirection direction;
//	long position;
//};

class APFuturesPositionCtrl :
	public APPositionCtrl
{
public:
	APFuturesPositionCtrl();
	~APFuturesPositionCtrl();

	virtual void init(std::string positionInfo);

	virtual void open(APTradeDirection direction, double price, long volume);
	virtual void close(APTradeDirection direction, double price, long volume);
	virtual void openAll(APTradeDirection direction, double price);
	virtual void closeAll(APTradeDirection direction, double price);

	virtual void cancel(APTradeType type, double price, APTradeDirection direction = TD_Buy);
	virtual void cancel(APTradeType type);

	virtual void onTradeDealt(APASSETID instrumentID, APTradeType type,  double price, long deltaVolume, APORDERID orderID, APTradeDirection direction = TD_Buy);
	virtual void onTradeCanceled(APASSETID instrumentID, APTradeType type, long volume, APORDERID orderID, APTradeDirection direction = TD_Buy);

	void setInstrumentID(APASSETID instrumentID);
	void setTradeDirection(APTradeDirection direction);

	// open/close directly
	void open(APASSETID instrumentID, APTradeDirection direction, double price, long volume, APOrderTimeCondition ot = OTC_GoodForDay);
	void close(APASSETID instrumentID, APTradeDirection direction, double price, long volume, APOrderTimeCondition ot = OTC_GoodForDay);
	// advanced open/close
	void open(APASSETID instrumentID, APTradeDirection direction,
		APOrderPriceType orderPriceType, double price,
		APOrderTimeCondition orderTimeCondition = OTC_GoodForDay, std::string date = "",
		APOrderVolumeCondition orderVolumeCondition = OVC_Any, long volume = 0, long minVolume = 0,
		APOrderContingentCondition orderContingentCondition = OCC_Immediately, double stopPrice = 0.0);
	void close(APASSETID instrumentID, APTradeDirection direction,
		APOrderPriceType orderPriceType, double price,
		APOrderTimeCondition orderTimeCondition = OTC_GoodForDay, std::string date = "",
		APOrderVolumeCondition orderVolumeCondition = OVC_Any, long volume = 0, long minVolume = 0,
		APOrderContingentCondition orderContingentCondition = OCC_Immediately, double stopPrice = 0.0);

	void setCloseTodayFirst(bool closeTDFirst);

	void setTrade(APTrade* trader);

private:
	bool m_closeTodayFirst;
};

