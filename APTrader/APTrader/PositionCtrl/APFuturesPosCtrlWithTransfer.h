#pragma once
#include "APFuturesPositionCtrl.h"

class APFuturesPosCtrlWithTransfer : public APFuturesPositionCtrl
{
public:
	APFuturesPosCtrlWithTransfer();
	~APFuturesPosCtrlWithTransfer();

	virtual void init(std::string positionInfo);

	void setTargetContractID(APASSETID instrumentID);
	virtual void onTradeDealt(APASSETID instrumentID, APTradeType type,  double price, long volume, APORDERID orderID, APTrendType trend = TT_Long);

	virtual void open(APTrendType type, double price, long volume);
	virtual void close(APTrendType type, double price, long volume);

	void beginTransfer();
	bool isTransferring();
	bool isTransferFinished();

	void transferContracts(double droppedContractPrice, double targetContractPrice, long volume);

protected:
	void checkTransferStatus();

protected:
	APASSETID m_targetContractID;
	bool m_isTransferring;
	bool m_finishTransferring;
	long m_positonNeedTransfer;
	long m_targetContractHoldPosition;
};

