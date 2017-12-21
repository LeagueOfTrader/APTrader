#pragma once
#include "APFuturesPositionCtrl.h"
class APFuturesCombinationPosCtrl :
	public APFuturesPositionCtrl
{
public:
	APFuturesCombinationPosCtrl();
	~APFuturesCombinationPosCtrl();

	virtual void init(std::string positionInfo);

	virtual void open(APTrendType type, double price, long volume);
	virtual void close(APTrendType type, double price, long volume);
	virtual void openAll(APTrendType type, double price);
	virtual void closeAll(APTrendType type, double price);

protected:
	APASSETID m_coInstrumentID;
	APTrendType m_coTrend;

	APInstrumentQuotation* m_coQuotation;

	long m_prVolume; // prime volume
	long m_coVolume; // combined volume

	long m_prUnitVol;
	long m_coUnitVol;
};

