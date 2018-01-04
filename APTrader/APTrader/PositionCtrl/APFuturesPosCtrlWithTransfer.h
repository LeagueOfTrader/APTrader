#pragma once
#include "APFuturesPositionCtrl.h"

class APFuturesPosCtrlWithTransfer : public APFuturesPositionCtrl
{
public:
	APFuturesPosCtrlWithTransfer();
	~APFuturesPosCtrlWithTransfer();

	virtual void init(std::string positionInfo);

	void setTargetContractID(APASSETID instrumentID);
	virtual void onTradeDealt(APASSETID instrumentID, APTradeType type,  double price, long volume, APORDERID orderID, APTradeDirection direction = TD_Buy);

	virtual void open(APTradeDirection direction, double price, long volume);
	virtual void close(APTradeDirection direction, double price, long volume);

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

