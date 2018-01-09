#include "APFuturesCTPTraderAgent.h"
#include "../../Utils/APLog.h"
#include "../../APAccountAssets.h"
#include "../../APTradeManager.h"
#include "../../Utils/APTimeUtility.h"

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

// parse
APOrderState parseOrderState(TThostFtdcOrderStatusType orderStatus) {
	APOrderState orderState = OS_None;
	switch (orderStatus) {
	case THOST_FTDC_OST_AllTraded:
		orderState = OS_AllTraded;
		break;
	case THOST_FTDC_OST_PartTradedQueueing:
		orderState = OS_PartTradedQueueing;
		break;
	case THOST_FTDC_OST_PartTradedNotQueueing:
		orderState = OS_PartTradedNotQueueing;
		break;
	case THOST_FTDC_OST_NoTradeQueueing:
		orderState = OS_NoTradeQueueing;
		break;
	case THOST_FTDC_OST_NoTradeNotQueueing:
		orderState = OS_NoTradeNotQueueing;
		break;
	case THOST_FTDC_OST_Canceled:
		orderState = OS_Canceled;
		break;
	case THOST_FTDC_OST_Unknown:
		orderState = OS_Unknown;
		break;
	case THOST_FTDC_OST_NotTouched:
		orderState = OS_NotTouched;
		break;
	case THOST_FTDC_OST_Touched:
		orderState = OS_Touched;
		break;
	default:
		break;
	}
	return orderState;
}

const std::string tradeFlowPath = "Data/CTP/tradeflow/";

APFuturesCTPTraderAgent::APFuturesCTPTraderAgent()
{
	m_traderApi = NULL;
	m_tradingDay = "";
	m_maxOrderRef = 0;
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

void APFuturesCTPTraderAgent::applyOrder(APTradeType tradeType, APASSETID instrumentID, double price, long volume, APORDERID localOrderID, APTradeDirection direction)
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
	
	if (direction == TD_Buy) {
		order.Direction = THOST_FTDC_D_Buy;
	}
	else if (direction == TD_Sell) {
		order.Direction = THOST_FTDC_D_Sell;
	}
	else
	{
		//error
		return;
	}

	if (tradeType == TT_Open) {
		
		order.CombOffsetFlag[0] = THOST_FTDC_OF_Open;
	}
	else if (tradeType == TT_Close) {
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

void APFuturesCTPTraderAgent::applyOrder(APTradeType tradeType, APASSETID instrumentID, double price, long volume, APORDERID localOrderID, APTradeDirection direction,
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

	if (direction == TD_Buy) {
		order.Direction = THOST_FTDC_D_Buy;
	}
	else if (direction == TD_Sell) {
		order.Direction = THOST_FTDC_D_Sell;
	}
	else
	{
		//error
		return;
	}

	if (tradeType == TT_Open) {
		order.CombOffsetFlag[0] = THOST_FTDC_OF_Open;
	}
	else if (tradeType == TT_Close) {
		order.CombOffsetFlag[0] = THOST_FTDC_OF_Close;
	}
	else if (tradeType == TT_CloseToday) {
		order.CombOffsetFlag[0] = THOST_FTDC_OF_CloseToday;
	}
	else if (tradeType == TT_CloseYesterday) {
		order.CombOffsetFlag[0] = THOST_FTDC_OF_CloseYesterday;
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
	strcpy(req.OrderSysID, sysID.c_str());

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

void APFuturesCTPTraderAgent::setMaxOrderRef(int id)
{
	m_maxOrderRef = id;
}

int APFuturesCTPTraderAgent::getMaxOrderRef()
{
	return m_maxOrderRef;
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
	//m_traderApi->ReqQryInvestorPositionDetail();
	return ret;
}

int APFuturesCTPTraderAgent::reqQryOrder(APASSETID instrumentID, APSYSTEMID sysID)
{
	if (m_traderApi == NULL) {
		return -1;
	}

	CThostFtdcQryOrderField req;
	memset(&req, 0, sizeof(req));
	strcpy(req.BrokerID, m_brokerID.c_str());
	strcpy(req.InvestorID, m_userID.c_str());
	strcpy(req.InstrumentID, instrumentID.c_str());
	strcpy(req.OrderSysID, sysID.c_str());

	int ret = m_traderApi->ReqQryOrder(&req, genReqID());
	return ret;
}

int APFuturesCTPTraderAgent::reqQryTrade(std::string tradeID, std::string startDate, std::string endDate)
{
	if (m_traderApi == NULL) {
		return -1;
	}

	CThostFtdcQryTradeField req;
	memset(&req, 0, sizeof(req));
	strcpy(req.TradeID, tradeID.c_str());

	if (startDate.length() > 0) {
		strcpy(req.TradeTimeStart, startDate.c_str());
	}

	if (endDate.length() > 0) {
		strcpy(req.TradeTimeEnd, endDate.c_str());
	}

	int ret = m_traderApi->ReqQryTrade(&req, genReqID());
	return ret;
}

int APFuturesCTPTraderAgent::reqQryInstrumentCommissionRate(APASSETID instrumentID)
{
	if (m_traderApi == NULL) {
		return -1;
	}

	CThostFtdcQryInstrumentCommissionRateField req;
	memset(&req, 0, sizeof(req));
	strcpy(req.BrokerID, m_brokerID.c_str());
	strcpy(req.InvestorID, m_userID.c_str());
	strcpy(req.InstrumentID, instrumentID.c_str());
	int ret = m_traderApi->ReqQryInstrumentCommissionRate(&req, genReqID());
	return ret;
}

int APFuturesCTPTraderAgent::reqQryInstrumentMarginRate(APASSETID instrumentID)
{
	if (m_traderApi == NULL) {
		return -1;
	}
	
	CThostFtdcQryInstrumentMarginRateField req;
	memset(&req, 0, sizeof(req));
	strcpy(req.BrokerID, m_brokerID.c_str());
	strcpy(req.InvestorID, m_userID.c_str());
	strcpy(req.InstrumentID, instrumentID.c_str());
	req.HedgeFlag = THOST_FTDC_HF_Speculation;

	int ret = m_traderApi->ReqQryInstrumentMarginRate(&req, genReqID());
	return ret;
}

void APFuturesCTPTraderAgent::onQryInstrumentPositionFinished(APASSETID instrumentID)
{
	APAccountAssets::getInstance()->onGetPositionData(instrumentID, m_positionInfo[instrumentID]);
}

void APFuturesCTPTraderAgent::onQryInstrumentPosition(APASSETID instrumentID, CThostFtdcInvestorPositionField * positionInfo)
{
	if (positionInfo == NULL) {
		return;
	}

	if (m_positionInfo.find(instrumentID) == m_positionInfo.end()) {
		std::vector<CThostFtdcInvestorPositionField> posInfoArr;
		m_positionInfo[instrumentID] = posInfoArr;
	}

	m_positionInfo[instrumentID].push_back(*positionInfo);
}

void APFuturesCTPTraderAgent::onRtnOrder(CThostFtdcOrderField * order)
{
	if (order == NULL) {
		return;
	}

	if (APTradeManager::getInstance()->inited() == false) {
		return;
	}

	APTrade* trader = APTradeManager::getInstance()->getTradeInstance();
	if (trader == NULL) {
		return;
	}

	std::string instrumentID = order->InstrumentID;
	APTradeType tradeType = TT_Num;
	if (order->CombOffsetFlag[0] == THOST_FTDC_OF_Open) {
		tradeType = TT_Open;
	}
	else if (order->CombOffsetFlag[0] == THOST_FTDC_OF_Close
			|| order->CombOffsetFlag[0] == THOST_FTDC_OF_CloseToday
			|| order->CombOffsetFlag[0] == THOST_FTDC_OF_CloseYesterday
			|| order->CombOffsetFlag[0] == THOST_FTDC_OF_ForceClose) {
		tradeType = TT_Close; // ��ʱֻ������ͨƽ��
	}

	APORDERID localOrderID = atoi(order->OrderLocalID);
	APSYSTEMID sysOrderID = order->OrderSysID;	
	APOrderState orderState = parseOrderState(order->OrderStatus);
	APTradeDirection direction = TD_Buy;
	if (order->Direction == THOST_FTDC_D_Buy) {
		direction = TD_Buy;
	}
	else if(order->Direction == THOST_FTDC_D_Sell){
		direction = TD_Sell;
	}
	long volumeTotal = order->VolumeTotal;
	long volumeTraded = order->VolumeTraded;
	APSYSTEMID tradeID = order->TraderID;

	trader->onOrderStatusChanged(instrumentID, tradeType, localOrderID, volumeTotal, volumeTraded, 
								orderState, sysOrderID, tradeID, direction);
}

void APFuturesCTPTraderAgent::onRtnTrade(CThostFtdcTradeField * info)
{
	if (info == NULL) {
		return;
	}

	if (APTradeManager::getInstance()->inited() == false) {
		return;
	}

	APTrade* trader = APTradeManager::getInstance()->getTradeInstance();
	if (trader == NULL) {
		return;
	}

	std::string instrumentID = info->InstrumentID;

	APTradeType tradeType = TT_Num;
	if (info->OffsetFlag == THOST_FTDC_OF_Open) {
		tradeType = TT_Open;
	}
	else if (info->OffsetFlag == THOST_FTDC_OF_Close
		|| info->OffsetFlag == THOST_FTDC_OF_CloseToday
		|| info->OffsetFlag == THOST_FTDC_OF_CloseYesterday
		|| info->OffsetFlag == THOST_FTDC_OF_ForceClose) {
		tradeType = TT_Close; // ��ʱֻ������ͨƽ��
	}

	double price = info->Price;
	long volume = info->Volume;

	APORDERID orderID = atoi(info->OrderLocalID);
	APSYSTEMID sysID = info->OrderSysID;
	APTradeDirection direction = TD_Buy;
	if (info->Direction == THOST_FTDC_D_Buy) {
		direction = TD_Buy;
	}
	else if (info->Direction == THOST_FTDC_D_Sell) {
		direction = TD_Sell;
	}

	trader->onTraded(instrumentID, tradeType, price, volume, orderID, sysID, direction);
}

void APFuturesCTPTraderAgent::onQryOrder(APORDERID localOrderID, CThostFtdcOrderField * pOrderInfo)
{
	if (pOrderInfo == NULL) {
		return;
	}

	m_orderInfo[localOrderID] = *pOrderInfo;
}

void APFuturesCTPTraderAgent::onQryOrderFinished(APORDERID orderID)
{
	if (APTradeManager::getInstance()->inited() == false) {
		return;
	}
	APTrade* trader = APTradeManager::getInstance()->getTradeInstance();
	if (trader == NULL) {
		return;
	}

	trader->onQueryOrder(orderID);
}

void APFuturesCTPTraderAgent::onQryOrderFailed(APORDERID orderID)
{
	if (APTradeManager::getInstance()->inited() == false) {
		return;
	}
	APTrade* trader = APTradeManager::getInstance()->getTradeInstance();
	if (trader == NULL) {
		return;
	}

	trader->onQueryOrderFailed(orderID);
}

APTradeOrderInfo APFuturesCTPTraderAgent::getOrderInfo(APORDERID orderID)
{
	APTradeOrderInfo info;
	memset(&info, 0, sizeof(info));
	if (m_orderInfo.find(orderID) != m_orderInfo.end()) {
		CThostFtdcOrderField& of = m_orderInfo[orderID];
		info.instrumentID = of.InstrumentID;
		if (of.Direction == THOST_FTDC_D_Buy) {
			info.direction = TD_Buy;
		}
		else if (of.Direction == THOST_FTDC_D_Sell) {
			info.direction = TD_Sell;
		}
		info.orderID = orderID;
		info.positionCtrlID = 0;
		info.price = of.LimitPrice;
		info.state = parseOrderState(of.OrderStatus);
		info.sysID = of.OrderSysID;
		info.tradeID = of.TraderID;
		if (of.CombOffsetFlag[0] == THOST_FTDC_OF_Open) {
			info.type = TT_Open;
		}
		else if (of.CombOffsetFlag[0] == THOST_FTDC_OF_Close
				|| of.CombOffsetFlag[0] == THOST_FTDC_OF_CloseYesterday
				|| of.CombOffsetFlag[0] == THOST_FTDC_OF_CloseToday) {
			info.type = TT_Close; // temp
		}
		info.volume = of.VolumeTotalOriginal;
		info.volumeSurplus = of.VolumeTotal;
		info.volumeTraded = of.VolumeTraded;
	}
	
	return info;
}

void APFuturesCTPTraderAgent::onQryTrade(APORDERID localOrderID, CThostFtdcTradeField * pTradeInfo)
{
	if (pTradeInfo == NULL) {
		return;
	}

	if (m_tradeInfo.find(localOrderID) == m_tradeInfo.end()) {
		std::vector<CThostFtdcTradeField> tradeArr;
		m_tradeInfo[localOrderID] = tradeArr;
	}

	m_tradeInfo[localOrderID].push_back(*pTradeInfo);
}

void APFuturesCTPTraderAgent::onQryTradeFinished(APORDERID localOrderID)
{
	//
}

void APFuturesCTPTraderAgent::onQryTradeFailed(APORDERID localOrderID)
{
	//
}

std::vector<APTradeDetailInfo> APFuturesCTPTraderAgent::getTradeInfo(APORDERID localOrderID, std::string startTime, std::string endTime)
{
	std::vector<APTradeDetailInfo> tradeInfo;
	if (m_tradeInfo.find(localOrderID) != m_tradeInfo.end()) {
		std::vector<CThostFtdcTradeField>& tradeArr = m_tradeInfo[localOrderID];
		for (int i = 0; i < tradeArr.size(); i++) {
			CThostFtdcTradeField& tf = tradeArr[i];
			std::string tradeDate = tf.TradeDate;
			std::string tradeTime = tf.TradeTime;
			std::string tradeDateTime = tradeDate + tradeTime;
			if (startTime.length() > 0) {
				if (APTimeUtility::compareDateTime(tradeDateTime, startTime) < 0) {
					continue;
				}
			}
			if (endTime.length() > 0) {
				if (APTimeUtility::compareDateTime(tradeDateTime, endTime) > 0) {
					continue;
				}
			}

			APTradeDetailInfo tdi;
			memset(&tdi, 0, sizeof(tdi));
			tdi.dateTime = tradeDateTime;
			if (tf.OffsetFlag == THOST_FTDC_OF_Open) {
				tdi.tradeType = TT_Open;
			}
			else if (tf.OffsetFlag == THOST_FTDC_OF_Close
				|| tf.OffsetFlag == THOST_FTDC_OF_CloseToday
				|| tf.OffsetFlag == THOST_FTDC_OF_CloseYesterday
				|| tf.OffsetFlag == THOST_FTDC_OF_ForceClose) {
				tdi.tradeType = TT_Close; // ��ʱֻ������ͨƽ��
			}

			tdi.price = tf.Price;
			tdi.volume = tf.Volume;

			tdi.localID = atoi(tf.OrderLocalID);
			tdi.sysID = tf.OrderSysID;
			APTradeDirection direction = TD_Buy;
			if (tf.Direction == THOST_FTDC_D_Buy) {
				direction = TD_Buy;
			}
			else if (tf.Direction == THOST_FTDC_D_Sell) {
				direction = TD_Sell;
			}

			tradeInfo.push_back(tdi);
		}
	}
	return tradeInfo;
}

void APFuturesCTPTraderAgent::onTradeFailed(APORDERID localID)
{
	if (APTradeManager::getInstance()->inited() == false) {
		return;
	}
	APTrade* trader = APTradeManager::getInstance()->getTradeInstance();
	if (trader == NULL) {
		return;
	}

	trader->onFailed(localID);
}

//void APFuturesCTPTraderAgent::onQryInstrumentPosition(CThostFtdcInvestorPositionField * pInvestorPosition)
//{
//	APAccountAssets::getInstance()->onGetPositionData(pInvestorPosition);
//}


#endif