#include "APFuturesCTPTraderAgent.h"

const std::string tradeFlowPath = "Data/CTP/tradeflow/";

APFuturesCTPTraderAgent::APFuturesCTPTraderAgent()
{
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
	m_traderApi->Join();
}

void APFuturesCTPTraderAgent::login()
{
}

CThostFtdcTraderApi * APFuturesCTPTraderAgent::getTraderApi()
{
	return m_traderApi;
}
