#pragma once
#include "../APMarco.h"

#ifdef USE_CTP

#include <string>
#include "../Common/Singleton.h"
#include "ThostFtdcMdApi.h"
#include "APFuturesCTPMDResponser.h"
#include "APFuturesCTPAgent.h"
#include <set>
#include <map>
#include "../APTypes.h"

class APFuturesCTPMDAgent : public APFuturesCTPAgent, public Singleton<APFuturesCTPMDAgent>
{
public:
	APFuturesCTPMDAgent();
	~APFuturesCTPMDAgent();

	virtual void initCTP();
	virtual void login();

	CThostFtdcMdApi* getMDApi();

	bool subscribeInstrument(APASSETID instrumentID);

	CThostFtdcDepthMarketDataField* getMarketData(APASSETID instrumentID);

	void onGetMarketData(CThostFtdcDepthMarketDataField* data);

private:
	CThostFtdcMdApi* m_mdApi;
	APFuturesCTPMDResponser* m_mdResponser;

	std::set<APASSETID> m_subscribedInstruments;
	std::map<APASSETID, CThostFtdcDepthMarketDataField*> m_marketData;
};

#endif