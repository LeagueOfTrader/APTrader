#pragma once
#include "../APMarco.h"

#ifdef USE_CTP

#include <string>
#include "../Common/Singleton.h"
#include "ThostFtdcTraderApi.h"
#include "APFuturesCTPTraderResponser.h"
#include "APFuturesCTPAgent.h"
#include "../APDef.h"
#include "../APTypes.h"

class APFuturesCTPTraderAgent : public APFuturesCTPAgent, public Singleton<APFuturesCTPTraderAgent>
{
public:
	APFuturesCTPTraderAgent();
	~APFuturesCTPTraderAgent();

	virtual void initCTP();
	virtual void login();

	CThostFtdcTraderApi* getTraderApi();

	void open(APORDERID orderID, APASSETID instrumentID, APTrendType trend, double price, long volume, APOrderTimeCondition ot = OTC_GoodForDay);
	void close(APORDERID orderID, APASSETID instrumentID, APTrendType trend, double price, long volume, APOrderTimeCondition ot = OTC_GoodForDay);
	void cancel(APSYSTEMID sysID);

private:
	CThostFtdcTraderApi* m_traderApi;
	APFuturesCTPTraderResponser* m_tradeResponser;
};

#endif
