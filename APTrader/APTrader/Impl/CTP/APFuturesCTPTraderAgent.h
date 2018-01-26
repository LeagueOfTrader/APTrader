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
	void cancelOrder(APASSETID instrumentID, int frontID, int sessionID, APSYSTEMID orderRef);
	void cancelOrder(APASSETID instrumentID, APSYSTEMID exchangeID, APSYSTEMID orderSysID);

	void setFrontID(TThostFtdcFrontIDType frontID);
	int getFrontID();
	//void setSessionID(TThostFtdcSessionIDType sessionID);
	void setMaxOrderRef(int id);
	int getMaxOrderRef();


	int reqUpdatePassword(std::string oldPassword, std::string newPassword);
	// 投资者结算结果确认
	int reqSettlementInfoConfirm();
	// 请求查询合约
	int reqQryInstrument(APASSETID instrumentID);
	// 请求查询资金账户
	int reqQryTradingAccount();
	// 请求查询仓位
	int reqQryInvestorPosition(APASSETID instrumentID);
	int reqQryAllInvestorPosition();
	int reqQryAllInvestorPositionDetail();
	//	订单查询
	int reqQryOrder(APASSETID instrumentID, APSYSTEMID sysID, APSYSTEMID exchangeID);
	int reqQryAllOrders();

	int reqQryTrade(APASSETID InstrumentID, std::string startDate = "", std::string endDate = "");
	int reqQryAllTrades(std::string startDate = "", std::string endDate = "");

	int reqQryInstrumentCommissionRate(APASSETID instrumentID);
	int reqQryInstrumentMarginRate(APASSETID instrumentID);

	// response	
	void onQryInstrumentPositionFinished();
	void onQryInstrumentPosition(APASSETID instrumentID, CThostFtdcInvestorPositionField* positionInfo);
	void onQryInstrumentPositionDetailFinished();
	void onQryInstrumentPositionDetail(APASSETID instrumentID, CThostFtdcInvestorPositionDetailField* detailInfo);

	// order & trade
	void onRtnOrder(CThostFtdcOrderField* order);
	void onRtnTrade(CThostFtdcTradeField* tradeInfo);

	void onQryOrder(APORDERID localOrderID, CThostFtdcOrderField* pOrderInfo);
	void onQryOrderFinished();
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
	//TThostFtdcSessionIDType m_sessionID;
	std::string m_tradingDay;

	std::map<APASSETID, std::vector<CThostFtdcInvestorPositionField>> m_positionInfo;
	std::map<APASSETID, std::vector<CThostFtdcInvestorPositionDetailField>> m_positionDetail;
	std::map<APORDERID, CThostFtdcOrderField> m_orderInfo;
	std::map<APORDERID, std::vector<CThostFtdcTradeField>> m_tradeInfo;

	int m_maxOrderRef;
};

#endif
