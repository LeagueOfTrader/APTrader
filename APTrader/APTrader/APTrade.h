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

struct APTradePositionData {
	APASSETID commodityID;
	APTrendType trend;
	UINT positionCtrlID;
};

struct APTradeOrderInfo {
	APORDERID orderID;
	APTradeType type;
	APASSETID commodityID;
	double price;
	long volume;
	APTrendType trend;

	APTradeOrderInfo() {
	}

	APTradeOrderInfo(APORDERID theOrderID, APTradeType theType, APASSETID theCommodityID, double thePrice, long theVolume, APTrendType theTrend) {
		orderID = theOrderID;
		type = theType;
		commodityID = theCommodityID;
		price = thePrice;
		volume = theVolume;
		trend = theTrend;
	}

	APTradeOrderInfo(const APTradeOrderInfo& info) {
		orderID = info.orderID;
		type = info.type;
		commodityID = info.commodityID;
		price = info.price;
		volume = info.volume;
		trend = info.trend;
	}
};

struct APTradeOrderPositionInfo{
	APTradeOrderInfo orderInfo;
	UINT positionCtrlID;

	APTradeOrderPositionInfo(UINT orderID, APTradeType type, APASSETID commodityID, double price, long volume, APTrendType trend, UINT posCtrlID) {
		orderInfo = { orderID, type, commodityID, price, volume, trend };
		positionCtrlID = posCtrlID;
	}
};

class APTrade
{
public:
	APTrade();
	~APTrade();

public:
	virtual void open(APASSETID commodityID, APTrendType trend, double price, long volume, APPositionCtrl* pc, APTradeOrderType ot = TOT_ValidTheDay);
	virtual void close(APASSETID commodityID, APTrendType trend, double price, long volume, APPositionCtrl* pc, APTradeOrderType ot = TOT_ValidTheDay);

	virtual void cancel(APASSETID commodityID, APTradeType type, APTrendType trend, double price, APPositionCtrl* pc);
	virtual void cancel(APASSETID commodityID, APTradeType type, APPositionCtrl* pc);
	virtual void cancelAll(APASSETID commodityID, APPositionCtrl* pc);
	
	virtual void onTradeFinished(APASSETID commodityID, APTradeType type, double price, long volume, APORDERID orderID, APTrendType trend = TT_Long);
	virtual void onTradeOrdered(APASSETID commodityID, APTradeType type, double price, long volume, APORDERID orderID, APTrendType trend = TT_Long);
	virtual void onFundChanged(APASSETID commodityID, APTradeType type, double variableFund, APORDERID orderID, APTrendType trend = TT_Long);

protected:
	virtual void open(APASSETID commodityID, APTrendType trend, double price, long volume, APTradeOrderType ot = TOT_ValidTheDay) = 0;
	virtual void close(APASSETID commodityID, APTrendType trend, double price, long volume, APTradeOrderType ot = TOT_ValidTheDay) = 0;

	virtual void cancel(APORDERID orderID) = 0;

private:
	std::vector<UINT> getRelatedOrders(APPositionCtrl* pc);

protected:
	std::list<APTradeOrderPositionInfo> m_ordersApplied;
	std::list<APTradeOrderPositionInfo> m_quickDealOrders;
	std::map<UINT, APTradeOrderInfo> m_ordersSubmitted;

	std::map<UINT, UINT> m_orderPosCtrlRelation;
	//std::map <UINT, std::set<UINT>> m_posCtrlOrders;
};

