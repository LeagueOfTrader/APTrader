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

	// Ͷ���߽�����ȷ��
	int reqSettlementInfoConfirm();
	// �����ѯ��Լ
	int reqQryInstrument(APASSETID instrumentID);
	// �����ѯ�ʽ��˻�
	int reqQryTradingAccount();
	// �����ѯ��λ
	int reqQryInvestorPosition(APASSETID instrumentID);
	//	������ѯ
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
