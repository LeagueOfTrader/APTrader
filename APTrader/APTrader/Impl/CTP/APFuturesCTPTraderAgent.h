#pragma once
#include "../../APMacro.h"

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
#include "../../APTrade.h"

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
	void setMaxOrderRef(int id);
	int getMaxOrderRef();


	int reqUpdatePassword(std::string oldPassword, std::string newPassword);
	// Ͷ���߽�����ȷ��
	int reqSettlementInfoConfirm();
	// �����ѯ��Լ
	int reqQryInstrument(APASSETID instrumentID);
	// �����ѯ�ʽ��˻�
	int reqQryTradingAccount();
	// �����ѯ��λ
	int reqQryInvestorPosition(APASSETID instrumentID);
	int reqQryAllInvestorPosition();
	int reqQryAllInvestorPositionDetail();
	//	������ѯ
	int reqQryOrder(APASSETID instrumentID, APSYSTEMID sysID);
	int reqQryAllOrders();

	int reqQryTrade(std::string tradeID, std::string startDate = "", std::string endDate = "");

	int reqQryInstrumentCommissionRate(APASSETID instrumentID);
	int reqQryInstrumentMarginRate(APASSETID instrumentID);

	// response	
	void onQryInstrumentPositionFinished(APASSETID instrumentID);
	void onQryInstrumentPosition(APASSETID instrumentID, CThostFtdcInvestorPositionField* positionInfo);

	// order & trade
	void onRtnOrder(CThostFtdcOrderField* order);
	void onRtnTrade(CThostFtdcTradeField* tradeInfo);

	void onQryOrder(APORDERID localOrderID, CThostFtdcOrderField* pOrderInfo);
	void onQryOrderFinished(APORDERID localOrderID);
	void onQryOrderFailed(APORDERID localOrderID);
	APTradeOrderInfo getOrderInfo(APORDERID orderID);

	void onQryTrade(APORDERID localOrderID, CThostFtdcTradeField* pTradeInfo);
	void onQryTradeFinished(APORDERID localOrderID);
	void onQryTradeFailed(APORDERID localOrderID);
	std::vector<APTradeDetailInfo> getTradeInfo(APORDERID localOrderID, std::string startTime = "", std::string endTime = "");

	void onTradeFailed(APORDERID localID);

private:
	CThostFtdcTraderApi* m_traderApi;
	APFuturesCTPTraderResponser* m_tradeResponser;
	TThostFtdcFrontIDType m_frontID;
	TThostFtdcSessionIDType m_sessionID;
	std::string m_tradingDay;

	std::map<APASSETID, std::vector<CThostFtdcInvestorPositionField>> m_positionInfo;
	std::map<APORDERID, CThostFtdcOrderField> m_orderInfo;
	std::map<APORDERID, std::vector<CThostFtdcTradeField>> m_tradeInfo;

	int m_maxOrderRef;
};

#endif
