#include "APFuturesCTPTraderAgent.h"
#include "../Utils/APLog.h"

#ifdef USE_CTP


// convertion
TThostFtdcOrderPriceTypeType convertOrderPriceType(APOrderPriceType opt) {
	TThostFtdcOrderPriceTypeType ret = THOST_FTDC_OPT_LimitPrice;
	switch (opt) {
	case OPT_AnyPrice:
		ret = THOST_FTDC_OPT_AnyPrice;
		break;
	case OPT_LimitPrice:
		ret = THOST_FTDC_OPT_LimitPrice;
		break;
	case OPT_BestPrice:
		ret = THOST_FTDC_OPT_BestPrice;
		break;
	case OPT_LastPrice:
		ret = THOST_FTDC_OPT_LastPrice;
		break;
	case OPT_LastPricePlusOneTicks:
		ret = THOST_FTDC_OPT_LastPricePlusOneTicks;
		break;
	case OPT_LastPricePlusTwoTicks:
		ret = THOST_FTDC_OPT_LastPricePlusTwoTicks;
		break;
	case OPT_LastPricePlusThreeTicks:
		ret = THOST_FTDC_OPT_LastPricePlusThreeTicks;
		break;
	case OPT_AskPrice1:
		ret = THOST_FTDC_OPT_AskPrice1;
		break;
	case OPT_AskPrice1PlusOneTicks:
		ret = THOST_FTDC_OPT_AskPrice1PlusOneTicks;
		break;
	case OPT_AskPrice1PlusTwoTicks:
		ret = THOST_FTDC_OPT_AskPrice1PlusTwoTicks;
		break;
	case OPT_AskPrice1PlusThreeTicks:
		ret = THOST_FTDC_OPT_AskPrice1PlusThreeTicks;
		break;
	case OPT_BidPrice1:
		ret = THOST_FTDC_OPT_BidPrice1;
		break;
	case OPT_BidPrice1PlusOneTicks:
		ret = THOST_FTDC_OPT_BidPrice1PlusOneTicks;
		break;
	case OPT_BidPrice1PlusTwoTicks:
		ret = THOST_FTDC_OPT_BidPrice1PlusTwoTicks;
		break;
	case OPT_BidPrice1PlusThreeTicks:
		ret = THOST_FTDC_OPT_BidPrice1PlusThreeTicks;
		break;
	case OPT_FiveLevelPrice:
		ret = THOST_FTDC_OPT_FiveLevelPrice;
		break;
	default:
		break;
	}
	return ret;
}

TThostFtdcTimeConditionType convertOrderTimeCondition(APOrderTimeCondition otc) {
	TThostFtdcTimeConditionType ret = THOST_FTDC_TC_GFD;
	switch (otc) {
	case OTC_GoodForDay:
		ret = THOST_FTDC_TC_GFD;
		break;
	case OTC_FAK:
	case OTC_FOK:
		ret = THOST_FTDC_TC_IOC;
		break;
	case OTC_GoodForAuction:
		ret = THOST_FTDC_TC_GFA;
		break;
	case OTC_GoodTillCancel:
		ret = THOST_FTDC_TC_GTC;
		break;
	case OTC_GoodForSection:
		ret = THOST_FTDC_TC_GFS;
		break;
	case OTC_GoodTillDate:
		ret = THOST_FTDC_TC_GTD;
	default:
		break;
	}

	return ret;
}

TThostFtdcVolumeConditionType convertOrderVolumeCondition(APOrderVolumeCondition ovc) {
	TThostFtdcVolumeConditionType ret = THOST_FTDC_VC_AV;
	switch (ovc) {
	case OVC_Any:
		ret = THOST_FTDC_VC_AV;
		break;
	case OVC_All:
		ret = THOST_FTDC_VC_CV;
		break;
	case OVC_Min:
		ret = THOST_FTDC_VC_MV;
		break;
	default:
		break;
	}

	return ret;
}

TThostFtdcContingentConditionType convertOrderContingentCondition(APOrderContingentCondition occ) {
	TThostFtdcContingentConditionType ret = THOST_FTDC_CC_Immediately;
	switch (occ) {
	case OCC_Immediately:
		ret = THOST_FTDC_CC_Immediately;
		break;
	case OCC_Touch:
		ret = THOST_FTDC_CC_Touch;
		break;
	case OCC_TouchProfit:
		ret = THOST_FTDC_CC_TouchProfit;
		break;
	case OCC_ParkedOrder:
		ret = THOST_FTDC_CC_ParkedOrder;
		break;
	case OCC_LastPriceGreaterThanStopPrice:
		ret = THOST_FTDC_CC_LastPriceGreaterThanStopPrice;
		break;
	case OCC_LastPriceGreaterEqualStopPrice:
		ret = THOST_FTDC_CC_LastPriceGreaterEqualStopPrice;
		break;
	case OCC_LastPriceLesserThanStopPrice:
		ret = THOST_FTDC_CC_LastPriceLesserThanStopPrice;
		break;
	case OCC_LastPriceLesserEqualStopPrice:
		ret = THOST_FTDC_CC_LastPriceLesserEqualStopPrice;
		break;
	case OCC_AskPriceGreaterThanStopPrice:
		ret = THOST_FTDC_CC_AskPriceGreaterThanStopPrice;
		break;
	case OCC_AskPriceGreaterEqualStopPrice:
		ret = THOST_FTDC_CC_AskPriceGreaterEqualStopPrice;
		break;
	case OCC_AskPriceLesserThanStopPrice:
		ret = THOST_FTDC_CC_AskPriceLesserThanStopPrice;
		break;
	case OCC_AskPriceLesserEqualStopPrice:
		ret = THOST_FTDC_CC_AskPriceLesserEqualStopPrice;
		break;
	case OCC_BidPriceGreaterThanStopPrice:
		ret = THOST_FTDC_CC_BidPriceGreaterThanStopPrice;
		break;
	case OCC_BidPriceGreaterEqualStopPrice:
		ret = THOST_FTDC_CC_BidPriceGreaterEqualStopPrice;
		break;
	case OCC_BidPriceLesserThanStopPrice:
		ret = THOST_FTDC_CC_BidPriceLesserThanStopPrice;
		break;
	case OCC_BidPriceLesserEqualStopPrice:
		ret = THOST_FTDC_CC_BidPriceLesserEqualStopPrice;
		break;
	default:
		break;
	}

	return ret;
}

const std::string tradeFlowPath = "Data/CTP/tradeflow/";

APFuturesCTPTraderAgent::APFuturesCTPTraderAgent()
{
	m_traderApi = NULL;
	m_tradingDay = "";
}


APFuturesCTPTraderAgent::~APFuturesCTPTraderAgent()
{
}

void APFuturesCTPTraderAgent::initCTP()
{
	m_traderApi = CThostFtdcTraderApi::CreateFtdcTraderApi(tradeFlowPath.c_str());
	m_tradeResponser = new APFuturesCTPTraderResponser();
	m_traderApi->RegisterSpi(m_tradeResponser);
	m_traderApi->RegisterFront((char*)m_tradeFront.c_str());
	m_traderApi->SubscribePrivateTopic(THOST_TERT_QUICK);
	m_traderApi->SubscribePublicTopic(THOST_TERT_QUICK);
	m_traderApi->Init();
	//m_traderApi->Join();
}

void APFuturesCTPTraderAgent::login()
{
	if (m_traderApi == NULL) {
		return;
	}

	CThostFtdcReqUserLoginField req;
	memset(&req, 0, sizeof(req));
	strcpy(req.BrokerID, m_brokerID.c_str());
	strcpy(req.UserID, m_userID.c_str());
	strcpy(req.Password, m_password.c_str());

	int ret = m_traderApi->ReqUserLogin(&req, genReqID());

	if (ret == 0) {
		APLogger->log("Req Login Success. ");
	}
	else{ 
		APLogger->log("Req Login Fail. ");
	}
}

void APFuturesCTPTraderAgent::onLogin()
{
	APFuturesCTPAgent::onLogin();

	if (m_traderApi == NULL) {
		return;
	}

	m_tradingDay = m_traderApi->GetTradingDay();
}

CThostFtdcTraderApi * APFuturesCTPTraderAgent::getTraderApi()
{
	return m_traderApi;
}

void APFuturesCTPTraderAgent::applyOrder(APTradeType tradeType, APASSETID instrumentID, double price, long volume, APORDERID localOrderID, APTrendType trend)
{
	if (m_traderApi == NULL) {
		return;
	}

	CThostFtdcInputOrderField order;
	memset(&order, 0, sizeof(order));
	strcpy(order.BrokerID, m_brokerID.c_str());
	strcpy(order.InvestorID, m_userID.c_str());
	char localID[32];
	itoa(localOrderID, localID, 10);
	strcpy(order.OrderRef, localID);
	
	if (trend == TT_Long) {
		order.Direction = THOST_FTDC_D_Buy;
	}
	else if (trend == TT_Short) {
		order.Direction = THOST_FTDC_D_Sell;
	}
	else
	{
		//error
		return;
	}

	if (tradeType == TDT_Open) {
		
		order.CombOffsetFlag[0] = THOST_FTDC_OF_Open;
	}
	else if (tradeType == TDT_Close) {
		order.Direction = THOST_FTDC_D_Sell;
		order.CombOffsetFlag[0] = THOST_FTDC_OF_Close;
	}
	else {
		// error type
		return;
	}
	order.OrderPriceType = THOST_FTDC_OPT_LimitPrice;	
	order.CombHedgeFlag[0] = THOST_FTDC_HF_Speculation;
	order.LimitPrice = price;
	order.VolumeCondition = THOST_FTDC_VC_AV;
	order.VolumeTotalOriginal = volume;
	order.TimeCondition = THOST_FTDC_TC_GFD;
	order.MinVolume = 1;
	order.ContingentCondition = THOST_FTDC_CC_Immediately;
	order.ForceCloseReason = THOST_FTDC_FCC_NotForceClose;
	order.IsAutoSuspend = 0;
	order.UserForceClose = 0;

	int ret = m_traderApi->ReqOrderInsert(&order, genReqID());
	if (ret == 0) {
		APLogger->log("Submit Order %s Success. ", localOrderID);
	}
	else {
		APLogger->log("Submit Order %s Fail! ", localOrderID);
	}
}

void APFuturesCTPTraderAgent::applyOrder(APTradeType tradeType, APASSETID instrumentID, double price, long volume, APORDERID localOrderID, APTrendType trend,
										APOrderPriceType orderPriceType, APOrderTimeCondition orderTimeCondition, std::string date, 
										APOrderVolumeCondition orderVolumeCondition, long minVolume, 
										APOrderContingentCondition orderContingentCondition, double stopPrice)
{
	if (m_traderApi == NULL) {
		return;
	}

	CThostFtdcInputOrderField order;
	memset(&order, 0, sizeof(order));
	strcpy(order.BrokerID, m_brokerID.c_str());
	strcpy(order.InvestorID, m_userID.c_str());
	char localID[32];
	itoa(localOrderID, localID, 10);
	strcpy(order.OrderRef, localID);

	if (trend == TT_Long) {
		order.Direction = THOST_FTDC_D_Buy;
	}
	else if (trend == TT_Short) {
		order.Direction = THOST_FTDC_D_Sell;
	}
	else
	{
		//error
		return;
	}

	if (tradeType == TDT_Open) {
		order.CombOffsetFlag[0] = THOST_FTDC_OF_Open;
	}
	else if (tradeType == TDT_Close) {
		order.CombOffsetFlag[0] = THOST_FTDC_OF_Close;
	}
	else {
		// error type
		return;
	}
	order.CombHedgeFlag[0] = THOST_FTDC_HF_Speculation;
	// price condition
	order.OrderPriceType = convertOrderPriceType(orderPriceType);	
	order.LimitPrice = price;
	// volume condition
	order.VolumeCondition = convertOrderVolumeCondition(orderVolumeCondition);

	order.MinVolume = minVolume;
	order.VolumeTotalOriginal = volume;
	// time condition
	order.TimeCondition = convertOrderTimeCondition(orderTimeCondition);
	strcpy(order.GTDDate, date.c_str());
	// contingent condition
	order.ContingentCondition = convertOrderContingentCondition(orderContingentCondition);
	order.StopPrice = stopPrice;
	// others
	order.ForceCloseReason = THOST_FTDC_FCC_NotForceClose;
	order.IsAutoSuspend = 0;
	order.UserForceClose = 0;
	//
	if (orderTimeCondition == OTC_FOK) {
		order.VolumeCondition = THOST_FTDC_VC_CV;
		order.OrderPriceType = THOST_FTDC_OPT_LimitPrice;
		order.ContingentCondition = THOST_FTDC_CC_Immediately;
	}
	else if (orderTimeCondition == OTC_FAK) {
		order.OrderPriceType = THOST_FTDC_OPT_LimitPrice;
		order.ContingentCondition = THOST_FTDC_CC_Immediately;
		if (order.VolumeCondition == THOST_FTDC_VC_CV) {
			order.VolumeCondition = (minVolume == 0 ? THOST_FTDC_VC_AV : THOST_FTDC_VC_MV);
		}
	}

	int ret = m_traderApi->ReqOrderInsert(&order, genReqID());
	if (ret == 0) {
		APLogger->log("Apply Order %s Success. ", localOrderID);
	}
	else {
		APLogger->log("Apply Order %s Fail! ", localOrderID);
	}
}

void APFuturesCTPTraderAgent::cancelOrder(APORDERID localOrderID, APSYSTEMID sysID)
{
	if (m_traderApi == NULL) {
		return;
	}

	CThostFtdcInputOrderActionField req;
	memset(&req, 0, sizeof(req));
	strcpy(req.BrokerID, m_brokerID.c_str());
	strcpy(req.InvestorID, m_userID.c_str());
	char strOrderID[32];
	itoa(localOrderID, strOrderID, 10);
	strcpy(req.OrderRef, strOrderID);
	req.FrontID = m_frontID;
	req.SessionID = m_sessionID;
	req.ActionFlag = THOST_FTDC_AF_Delete;
	char remoteID[32];
	itoa(sysID, remoteID, 10);
	strcpy(req.OrderSysID, remoteID);

	int ret = m_traderApi->ReqOrderAction(&req, genReqID());
	if (ret == 0) {
		APLogger->log("Apply Cancel %d Success. ", sysID);
	}
	else {
		APLogger->log("Apply Cancel %d Failed. ", sysID);
	}
}

void APFuturesCTPTraderAgent::setFrontID(TThostFtdcFrontIDType frontID)
{
	m_frontID = frontID;
}

void APFuturesCTPTraderAgent::setSessionID(TThostFtdcSessionIDType sessionID)
{
	m_sessionID = sessionID;
}

int APFuturesCTPTraderAgent::reqSettlementInfoConfirm()
{
	if (m_traderApi == NULL) {
		return -1;
	}

	CThostFtdcSettlementInfoConfirmField req;
	memset(&req, 0, sizeof(req));
	strcpy(req.BrokerID, m_brokerID.c_str());
	strcpy(req.InvestorID, m_userID.c_str());

	int ret = m_traderApi->ReqSettlementInfoConfirm(&req, genReqID());
	return ret;
}

int APFuturesCTPTraderAgent::reqQryInstrument(APASSETID instrumentID)
{
	if (m_traderApi == NULL) {
		return -1;
	}

	CThostFtdcQryInstrumentField req;
	memset(&req, 0, sizeof(req));
	strcpy(req.InstrumentID, instrumentID.c_str());

	int ret = m_traderApi->ReqQryInstrument(&req, genReqID());
	return ret;
}

int APFuturesCTPTraderAgent::reqQryTradingAccount()
{
	if (m_traderApi == NULL) {
		return -1;
	}

	CThostFtdcQryTradingAccountField req;
	memset(&req, 0, sizeof(req));
	strcpy(req.BrokerID, m_brokerID.c_str());
	strcpy(req.InvestorID, m_userID.c_str());
	
	int ret = m_traderApi->ReqQryTradingAccount(&req, genReqID());
	return ret;
}

int APFuturesCTPTraderAgent::reqQryInvestorPosition(APASSETID instrumentID)
{
	if (m_traderApi == NULL) {
		return -1;
	}

	CThostFtdcQryInvestorPositionField req;
	memset(&req, 0, sizeof(req));
	strcpy(req.BrokerID, m_brokerID.c_str());
	strcpy(req.InvestorID, m_userID.c_str());
	strcpy(req.InstrumentID, instrumentID.c_str());

	int ret = m_traderApi->ReqQryInvestorPosition(&req, genReqID());
	return ret;
}

#endif