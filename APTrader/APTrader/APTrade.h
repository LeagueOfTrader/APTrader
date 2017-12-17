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

struct APTradePositionData {
	APASSETID instrumentID;
	APTrendType trend;
	UINT positionCtrlID;
};

struct APTradeOrderInfo {
	APORDERID orderID;
	APTradeType type;
	APASSETID instrumentID;
	double price;
	long volume;
	APTrendType trend;
	APTradeState state;
	APSYSTEMID sysID;

	APTradeOrderInfo() {
	}

	APTradeOrderInfo(APORDERID theOrderID, APTradeType theType, APASSETID theInstrumentID, double thePrice, long theVolume, 
					APTrendType theTrend, APTradeState theState, APSYSTEMID theSysID) {
		orderID = theOrderID;
		type = theType;
		instrumentID = theInstrumentID;
		price = thePrice;
		volume = theVolume;
		trend = theTrend;
		state = theState;
		sysID = theSysID;
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
	}
};

struct APTradeOrderPositionInfo{
	APTradeOrderInfo orderInfo;
	UINT positionCtrlID;

	APTradeOrderPositionInfo() {

	}

	APTradeOrderPositionInfo(UINT orderID, APTradeType type, APASSETID instrumentID, double price, long volume, APTrendType trend, UINT posCtrlID) {
		orderInfo = { orderID, type, instrumentID, price, volume, trend, TS_Apply, 0 };
		positionCtrlID = posCtrlID;
	}
};

class APTrade
{
public:
	APTrade();
	~APTrade();

public:
	virtual APORDERID open(APASSETID instrumentID, APTrendType trend, double price, long volume, 
							APPositionCtrl* pc, APOrderPriceType orderPriceType = OPT_LimitPrice, APOrderTimeCondition ot = OTC_GoodForDay);
	virtual APORDERID close(APASSETID instrumentID, APTrendType trend, double price, long volume, 
							APPositionCtrl* pc, APOrderPriceType orderPriceType = OPT_LimitPrice, APOrderTimeCondition ot = OTC_GoodForDay);

	virtual void cancel(APASSETID instrumentID, APTradeType type, APTrendType trend, double price, APPositionCtrl* pc);
	virtual void cancel(APASSETID instrumentID, APTradeType type, APPositionCtrl* pc);
	virtual void cancelAll(APASSETID instrumentID, APPositionCtrl* pc);
	virtual void cancel(APORDERID sysID, APPositionCtrl* pc);
	
	virtual void onTradeDealt(APASSETID instrumentID, APTradeType type, double price, long volume, APORDERID orderID, 
								APTradeState state, APSYSTEMID sysID, APTrendType trend = TT_Long);
	virtual void onTradeOrdered(APASSETID instrumentID, APTradeType type, double price, long volume, APORDERID orderID, 
								APTradeState state, APSYSTEMID sysID, APTrendType trend = TT_Long);
	virtual void onFundChanged(APASSETID instrumentID, APTradeType type, double variableFund, APORDERID orderID, APTrendType trend = TT_Long);

	bool getOrderInfo(APORDERID orderID, APTradeOrderInfo& orderInfo);

protected:
	virtual void open(APORDERID orderID, APASSETID instrumentID, APTrendType trend, double price, long volume, APOrderPriceType orderPriceType = OPT_LimitPrice, APOrderTimeCondition ot = OTC_GoodForDay) = 0;
	virtual void close(APORDERID orderID, APASSETID instrumentID, APTrendType trend, double price, long volume, APOrderPriceType orderPriceType = OPT_LimitPrice, APOrderTimeCondition ot = OTC_GoodForDay) = 0;

	virtual void cancel(APSYSTEMID sysID) = 0;

	APORDERID generateOrderID();

private:
	std::vector<UINT> getRelatedOrders(APPositionCtrl* pc);

protected:
	std::map<APORDERID, APTradeOrderPositionInfo> m_ordersApplied;
	std::map<APORDERID, APTradeOrderPositionInfo> m_quickDealOrders;
	std::map<APORDERID, APTradeOrderInfo> m_ordersConfirmed;

	std::map<APORDERID, UINT> m_orderPosCtrlRelation;
	//std::map <UINT, std::set<UINT>> m_posCtrlOrders;
	APIntAccumulator* m_idAccumulator;
};

