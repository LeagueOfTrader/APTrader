#include "APFuturesCTPAgent.h"
#include "../Utils/APJsonReader.h"

const std::string ctpCfgFile = "Data/CTP/CTPAccount.cfg";
const std::string ctpParamFile = "Data/CTP/CTPParam.cfg";

APFuturesCTPAgent::APFuturesCTPAgent()
{
	m_idAccumulator = new APIntAccumulator();
	m_loginSuccess = false;

	m_initCallback = NULL;
}


APFuturesCTPAgent::~APFuturesCTPAgent()
{
	if (m_idAccumulator != NULL) {
		delete m_idAccumulator;
		m_idAccumulator = NULL;
	}
}

void APFuturesCTPAgent::init()
{
	readAccount();
	readParam();

	initCTP();
}

void APFuturesCTPAgent::initCTP()
{
}

void APFuturesCTPAgent::login()
{
}

void APFuturesCTPAgent::onLogin()
{
	m_loginSuccess = true;
	if (m_initCallback != NULL) {
		m_initCallback();
	}
}

bool APFuturesCTPAgent::isLogin()
{
	return m_loginSuccess;
}

void APFuturesCTPAgent::registerInitCallback(InitCallback callback)
{
	m_initCallback = callback;
}

void APFuturesCTPAgent::readAccount()
{
	APJsonReader jr;
	jr.initWithFile(ctpCfgFile);

	m_brokerID = jr.getStrValue("BrokerID");
	m_userID = jr.getStrValue("UserID");
	m_password = jr.getStrValue("Password");
}

void APFuturesCTPAgent::readParam()
{
	APJsonReader jr;
	jr.initWithFile(ctpParamFile);

	std::string ctpStr = "StdCTP";
	m_marketFront = jr.getArrayFieldStrValue(ctpStr, "MarketFront", 0);
	m_tradeFront = jr.getArrayFieldStrValue(ctpStr, "TradeFront", 0);
}

int APFuturesCTPAgent::genReqID()
{
	if (m_idAccumulator != NULL) {
		return m_idAccumulator->generateID();
	}

	return -1;
}
