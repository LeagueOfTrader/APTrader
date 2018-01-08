#pragma once
#include "../../APMarco.h"

#ifdef USE_CTP

#include <string>
#include "../../Common/Singleton.h"
#include "ThostFtdcTraderApi.h"
#include "APFuturesCTPTraderResponser.h"
#include "APFuturesCTPAgent.h"
#include "../../APDef.h"
#include "../../APTypes.h"
#include <map>
#include <vector>

class APFuturesCTPTraderAgent : public APFuturesCTPAgent, public Singleton<APFuturesCTPTraderAgent>
{
public:
	APFuturesCTPTraderAgent();
	~APFuturesCTPTraderAgent();

	virtual void initCTP();
	virtual void login();
	virtual void onLogin();

	CThostFtdcTraderApi* getTraderApi();

	void applyOrder(APTradeType tradeType, APASSETID instrumentID, double price, long volume, APORDERID localOrderID, APTradeDirection direction);
	void applyOrder(APTradeType tradeType, APASSETID instrumentID, double price, long volume, APORDERID localOrderID, APTradeDirection direction,
		APOrderPriceType orderPriceType,
		APOrderTimeCondition orderTimeCondition = OTC_GoodForDay, std::string date = "",
		APOrderVolumeCondition orderVolumeCondition = OVC_Any, long minVolume = 0,
		APOrderContingentCondition orderContingentCondition = OCC_Immediately, double stopPrice = 0.0);
	void cancelOrder(APORDERID localOrderID, APSYSTEMID sysID);

	void setFrontID(TThostFtdcFrontIDType frontID);
	void setSessionID(TThostFtdcSessionIDType sessionID);

	// 投资者结算结果确认
	int reqSettlementInfoConfirm();
	// 请求查询合约
	int reqQryInstrument(APASSETID instrumentID);
	// 请求查询资金账户
	int reqQryTradingAccount();
	// 请求查询仓位
	int reqQryInvestorPosition(APASSETID instrumentID);
	//	订单查询
	int reqQryOrder(APASSETID instrumentID, APSYSTEMID sysID);
	int reqQryTrade(std::string tradeID);

	int reqQryInstrumentCommissionRate(APASSETID instrumentID);
	int reqQryInstrumentMarginRate(APASSETID instrumentID);

	// response	
	void onQryInstrumentPositionFinished(APASSETID instrumentID);
	void onQryInstrumentPosition(APASSETID instrumentID, CThostFtdcInvestorPositionField* positionInfo);

private:
	CThostFtdcTraderApi* m_traderApi;
	APFuturesCTPTraderResponser* m_tradeResponser;
	TThostFtdcFrontIDType m_frontID;
	TThostFtdcSessionIDType m_sessionID;
	std::string m_tradingDay;

	std::map<APASSETID, std::vector<CThostFtdcInvestorPositionField>> m_positionInfo;
};

#endif
