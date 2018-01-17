#pragma once
#include "APDef.h"
#include "APTypes.h"

struct APPositionData {
	std::string instrumentID;
	APTradeDirection direction;
	long holdPosition;
	long longFrozenPosition;
	long shortFrozenPosition;
	//
	long yesterdayPosition;
	long todayPosition;

	void assign(const APPositionData& refData) {
		instrumentID = refData.instrumentID;
		direction = refData.direction;
		holdPosition = refData.holdPosition;
		longFrozenPosition = refData.longFrozenPosition;
		shortFrozenPosition = refData.shortFrozenPosition;
		yesterdayPosition = refData.yesterdayPosition;
		todayPosition = refData.todayPosition;
	}

	bool capable(const APPositionData& posData) {
		if (holdPosition < posData.holdPosition) {
			return false;
		}
		if (longFrozenPosition < posData.longFrozenPosition) {
			return false;
		}
		if (shortFrozenPosition < posData.shortFrozenPosition) {
			return false;
		}

		return true;
	}

	void handle(const APPositionData& posData) {
		holdPosition -= posData.holdPosition;
		longFrozenPosition -= posData.longFrozenPosition;
		shortFrozenPosition -= posData.shortFrozenPosition;
	}
};

struct APOrderIndexInfo {
	APORDERID localID;
	APASSETID instrumentID;
	//APTradeDirection direction;
	APSYSTEMID sysID;
};

struct APTradeDetailInfo : APOrderIndexInfo {
	APTradeDirection direction;
	double price;
	long volume;
	std::string dateTime;
	APTradeType tradeType;
};

struct APOrderRecordInfo : APOrderIndexInfo {
	UINT positionCtrlID;
	APTradeDirection direction;
	APOrderState state;
	long volumeSurplus;
	std::string recordTime;
};


struct APTradeOrderInfo {
	APORDERID orderID;
	APTradeType type;
	APASSETID instrumentID;
	double price;
	long volume;
	APTradeDirection direction;
	APOrderState state;
	APSYSTEMID sysID;
	UINT positionCtrlID;

	//
	APSYSTEMID tradeID;
	long volumeTraded;
	long volumeSurplus;

	APTradeOrderInfo() {
		tradeID = "";
		volumeTraded = 0;
		volumeSurplus = 0;
	}

	APTradeOrderInfo(APORDERID theOrderID, APTradeType theType, APASSETID theInstrumentID, double thePrice, long theVolume,
		APTradeDirection theDir, APOrderState theState, APSYSTEMID theSysID, UINT thePositionCtrlID) {
		orderID = theOrderID;
		type = theType;
		instrumentID = theInstrumentID;
		price = thePrice;
		volume = theVolume;
		direction = theDir;
		state = theState;
		sysID = theSysID;
		positionCtrlID = thePositionCtrlID;

		tradeID = "";
		volumeTraded = 0;
		volumeSurplus = 0;
	}

	APTradeOrderInfo(const APTradeOrderInfo& info) {
		orderID = info.orderID;
		type = info.type;
		instrumentID = info.instrumentID;
		price = info.price;
		volume = info.volume;
		direction = info.direction;
		state = info.state;
		sysID = info.sysID;
		positionCtrlID = info.positionCtrlID;

		tradeID = info.tradeID;
		volumeTraded = info.volumeTraded;
		volumeSurplus = info.volumeSurplus;
	}
};

struct APBidInfo {
	double price;
	long volume;
};