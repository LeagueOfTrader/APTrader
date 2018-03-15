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

	void init(APASSETID instrument, APTradeDirection dir) {
		clear();

		instrumentID = instrument;
		direction = dir;
	}

	void clear() {
		instrumentID = "";
		direction = TD_Buy;
		holdPosition = 0;
		longFrozenPosition = 0;
		shortFrozenPosition = 0;
		yesterdayPosition = 0;
		todayPosition = 0;
	}

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

	void shift() {
		yesterdayPosition += todayPosition;
		todayPosition = 0;
	}

	void append(APTradeDirection dir, long vol) {
		if (dir == direction) {
			holdPosition += vol;
			todayPosition += vol;
		}
		else {
			if (holdPosition == 0) {
				holdPosition = vol;
				todayPosition = vol;
				direction = dir;
			}
		}
	}

	void substract(APTradeDirection dir, long vol, bool todayFirst = false) {
		if (dir == direction) {
			holdPosition -= vol;
			if (todayFirst) {
				if (todayPosition >= vol) {
					todayPosition -= vol;
				}
				else {
					vol -= todayPosition;
					todayPosition = 0;
					yesterdayPosition -= vol;
				}
			}
			else {
				if (yesterdayPosition >= vol) {
					yesterdayPosition -= vol;
				}
				else {
					vol -= yesterdayPosition;
					yesterdayPosition = 0;
					todayPosition -= vol;
				}
			}
		}
		else {
			// it seems not to happen
		}
	}
};

struct APOrderIndexInfo {
	APORDERID localID;
	APASSETID instrumentID;
	//APTradeDirection direction;
	APSYSTEMID sysID;
	APSYSTEMID orderRef;
	APSYSTEMID exchangeID;	
	int sessionID;
	int frontID;
};

struct APTradeDetailInfo : APOrderIndexInfo {
	APTradeDirection direction;
	double price;
	long volume;
	std::string dateTime;
	APTradeType tradeType;
};

//struct APOrderRecordInfo : APOrderIndexInfo {
//	UINT positionCtrlID;
//	APTradeDirection direction;
//	APOrderState state;
//	long volumeSurplus;
//	std::string recordTime;
//};


struct APTradeOrderInfo {
	APORDERID orderID;
	APTradeType type;
	APASSETID instrumentID;
	double price;
	long volume;
	APTradeDirection direction;
	APOrderState state;
	APSYSTEMID sysID;
	APSYSTEMID orderRef;
	APSYSTEMID exchangeID;
	int sessionID;
	int frontID;
	UINT observerID;

	//
	//APSYSTEMID tradeID;
	long volumeTraded;
	long volumeSurplus;

	APTradeOrderInfo() {
		//tradeID = "";
		orderRef = "";
		exchangeID = "";
		sessionID = 0;
		frontID = 0;
		volumeTraded = 0;
		volumeSurplus = 0;
	}

	APTradeOrderInfo(APORDERID theOrderID, APTradeType theType, APASSETID theInstrumentID, double thePrice, long theVolume,
		APTradeDirection theDir, APOrderState theState, UINT theObID) {
		orderID = theOrderID;
		type = theType;
		instrumentID = theInstrumentID;
		price = thePrice;
		volume = theVolume;
		direction = theDir;
		state = theState;
		//sysID = theSysID;
		observerID = theObID;

		//tradeID = "";
		sysID = "";
		orderRef = "";
		exchangeID = "";
		sessionID = 0;
		frontID = 0;
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
		orderRef = info.orderRef;
		exchangeID = info.exchangeID;
		sessionID = info.sessionID;
		frontID = info.frontID;
		observerID = info.observerID;
				
		volumeTraded = info.volumeTraded;
		volumeSurplus = info.volumeSurplus;
	}
};

struct APBidInfo {
	double price;
	long volume;
};