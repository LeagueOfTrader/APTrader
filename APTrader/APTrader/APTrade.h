#pragma once
#include "APDef.h"
//#include "APPositionCtrl.h"
#include <string>
#include <map>
#include <vector>
#include <list>
#include "APTypes.h"
#include <set>

#define UNDISTURBED_ORDER_ID 0
#define MATCH_ANY_ORDER_ID 0xFEEEFEEE

class APPositionCtrl;
class APIntAccumulator;

//struct APTradePositionData {
//	APASSETID instrumentID;
//	APTrendType trend;
//	UINT positionCtrlID;
//};

struct APTradeOrderInfo {
	APORDERID orderID;
	APTradeType type;
	APASSETID instrumentID;
	double price;
	long volume;
	APTrendType trend;
	APTradeState state;
	APSYSTEMID sysID;
	UINT positionCtrlID;

	APTradeOrderInfo() {
	}

	APTradeOrderInfo(APORDERID theOrderID, APTradeType theType, APASSETID theInstrumentID, double thePrice, long theVolume, 
					APTrendType theTrend, APTradeState theState, APSYSTEMID theSysID, UINT thePositionCtrlID) {
		orderID = theOrderID;
		type = theType;
		instrumentID = theInstrumentID;
		price = thePrice;
		volume = theVolume;
		trend = theTrend;
		state = theState;
		sysID = theSysID;
		positionCtrlID = thePositionCtrlID;
	}

	APTradeOrderInfo(const APTradeOrderInfo& info) {
		orderID = info.orderID;
		type = info.type;
		instrumentID = info.instrumentID;
		price = info.price;
		volume = info.volume;
		trend = info.trend;
		state = info.state;
		sysID = info.sysID;
		positionCtrlID = info.positionCtrlID;
	}
};

class APTrade
{
public:
	APTrade();
	~APTrade();

public:
	virtual APORDERID open(APASSETID instrumentID, APTrendType trend, double price, long volume, 
							APPositionCtrl* pc, APOrderTimeCondition ot = OTC_GoodForDay);
	virtual APORDERID close(APASSETID instrumentID, APTrendType trend, double price, long volume, 
							APPositionCtrl* pc, APOrderTimeCondition ot = OTC_GoodForDay);

	virtual APORDERID open(APASSETID instrumentID, APTrendType trend,
							APOrderPriceType orderPriceType, double price, APPositionCtrl* pc,
							APOrderTimeCondition orderTimeCondition = OTC_GoodForDay, std::string date = "",
							APOrderVolumeCondition orderVolumeCondition = OVC_Any, long volume = 0, long minVolume = 0,
							APOrderContingentCondition orderContingentCondition = OCC_Immediately, double stopPrice = 0.0);
	virtual APORDERID close(APASSETID instrumentID, APTrendType trend,
							APOrderPriceType orderPriceType, double price, APPositionCtrl* pc,
							APOrderTimeCondition orderTimeCondition = OTC_GoodForDay, std::string date = "",
							APOrderVolumeCondition orderVolumeCondition = OVC_Any, long volume = 0, long minVolume = 0,
							APOrderContingentCondition orderContingentCondition = OCC_Immediately, double stopPrice = 0.0);

	virtual void cancel(APASSETID instrumentID, APTradeType type, APTrendType trend, double price, APPositionCtrl* pc);
	virtual void cancel(APASSETID instrumentID, APTradeType type, APPositionCtrl* pc);
	virtual void cancelAll(APASSETID instrumentID, APPositionCtrl* pc);
	virtual void cancel(APORDERID orderID, APPositionCtrl* pc);
	
	virtual void onTradeDealt(APASSETID instrumentID, APTradeType type, double price, long volume, APORDERID orderID, 
								APTradeState state, APSYSTEMID sysID, APTrendType trend = TT_Long);
	virtual void onTradeOrdered(APASSETID instrumentID, APTradeType type, double price, long volume, APORDERID orderID, 
								APTradeState state, APSYSTEMID sysID, APTrendType trend = TT_Long);
	
	virtual void onTradeCanceled(APORDERID orderID, APSYSTEMID sysID = 0);

	virtual void onFundChanged(APASSETID instrumentID, APTradeType type, double variableFund, APORDERID orderID, APTrendType trend = TT_Long);

	bool getOrderInfo(APORDERID orderID, APTradeOrderInfo& orderInfo);

	void setOrderIDBase(APORDERID base);

protected:
	virtual void open(APORDERID orderID, APASSETID instrumentID, APTrendType trend, double price, long volume, 
						APOrderTimeCondition ot = OTC_GoodForDay) = 0;
	virtual void close(APORDERID orderID, APASSETID instrumentID, APTrendType trend, double price, long volume,
						APOrderTimeCondition ot = OTC_GoodForDay) = 0;
	virtual void cancel(APORDERID orderID) = 0;

	virtual void open(APASSETID instrumentID, APORDERID localOrderID, APTrendType trend,
						APOrderPriceType orderPriceType, double price,
						APOrderTimeCondition orderTimeCondition = OTC_GoodForDay, std::string date = "",
						APOrderVolumeCondition orderVolumeCondition = OVC_Any, long volume = 0, long minVolume = 0,
						APOrderContingentCondition orderContingentCondition = OCC_Immediately, double stopPrice = 0.0) = 0;
	virtual void close(APASSETID instrumentID, APORDERID localOrderID, APTrendType trend,
						APOrderPriceType orderPriceType, double price,
						APOrderTimeCondition orderTimeCondition = OTC_GoodForDay, std::string date = "",
						APOrderVolumeCondition orderVolumeCondition = OVC_Any, long volume = 0, long minVolume = 0,
						APOrderContingentCondition orderContingentCondition = OCC_Immediately, double stopPrice = 0.0) = 0;
	

	APORDERID generateOrderID();
	void removeLocalOrder(APORDERID orderID);
	APSYSTEMID getSysIDByOrder(APORDERID orderID);

private:
	std::vector<UINT> getRelatedOrders(APPositionCtrl* pc);
	APPositionCtrl* getPositionCtrlByOrder(APORDERID orderID);

protected:
	std::map<APORDERID, APTradeOrderInfo> m_localOrders;

	//std::map<APORDERID, UINT> m_orderPosCtrlRelation;
	//std::map <UINT, std::set<UINT>> m_posCtrlOrders;
	APIntAccumulator* m_idAccumulator;
};

