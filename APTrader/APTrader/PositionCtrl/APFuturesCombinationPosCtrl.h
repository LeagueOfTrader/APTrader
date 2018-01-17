#pragma once
#include "APFuturesPositionCtrl.h"

struct APCombinationOperationData {
	long prVolume = 0;
	long coVolume = 0;
	long prTarget = 0;
	long coTarget = 0;
	//long prOrdVol = 0;
	//long coOrdVol = 0;
	long groupCount = 0;

	void setTarget(long pt, long ct, long count) {
		prTarget = pt;
		coTarget = ct;
		groupCount = count;
	}

	bool hasTarget() {
		if (prTarget == 0 || coTarget == 0) {
			return false;
		}

		return true;
	}

	bool isFinishTarget() {
		if (prTarget == 0 || coTarget == 0) {
			return false;
		}

		return (prVolume == prTarget) && (coVolume == coTarget);
	}

	void reset() {
		prVolume = 0;
		coVolume = 0;
		prTarget = 0;
		coTarget = 0;
		//prOrdVol = 0;
		//coOrdVol = 0;
		groupCount = 0;
	}
};

class APFuturesCombinationPosCtrl :
	public APFuturesPositionCtrl
{
public:
	APFuturesCombinationPosCtrl();
	~APFuturesCombinationPosCtrl();

	virtual void initWithData(std::string positionInfo);

	virtual std::vector<APPositionData> getHoldPositionDetail();
	virtual void setHoldAmount(APASSETID instrumentID, long amount);
	std::pair<std::vector<APASSETID>, std::vector<long>> getCombinationUnitVol();

	virtual void openPosition(APTradeDirection direction, double price, long volume); // ignore price in this class
	virtual void closePosition(APTradeDirection direction, double price, long volume);
	virtual void openFullPosition(APTradeDirection direction, double price);
	virtual void closeOffPosition(APTradeDirection direction, double price);

	virtual void cancel(APTradeType type);

	virtual void onTradeDealt(APASSETID instrumentID, APTradeType type, double price, long deltaVolume, APORDERID orderID, APTradeDirection direction = TD_Buy);
	virtual void onTradeCanceled(APASSETID instrumentID, APTradeType type, long volume, APORDERID orderID, APTradeDirection direction = TD_Buy);

	virtual void correctPosition();

protected:
	virtual Json::Value serializeToJsonValue();
	virtual void deserialize(std::string str);

private:
	void openPrPosition();
	void openCoPosition();
	void closePrPosition();
	void closeCoPosition();

	//void onFinishOpenOperation();
	//void onFinishCloseOperation();

protected:
	APASSETID m_coInstrumentID;
	APTradeDirection m_coDirectionType;

	APInstrumentQuotation* m_coQuotation;

	long m_prVolume; // prime volume
	long m_coVolume; // combined volume

	long m_prUnitVol;
	long m_coUnitVol;

	// cur operation
	APCombinationOperationData m_curOpenOperation;
	APCombinationOperationData m_curCloseOperation;
};

