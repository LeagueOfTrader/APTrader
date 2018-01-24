#include "APFuturesCTPAgent.h"
#include "../../Utils/APJsonReader.h"

#include "APMacroCTP.h"

const std::string ctpCfgFile = "Data/CTP/CTPAccount.cfg";
#ifdef SIMNOW
const std::string ctpParamFile = "Data/CTP/CTPTestParam.cfg";
#else
const std::string ctpParamFile = "Data/CTP/CTPParam.cfg";
#endif

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

std::string APFuturesCTPAgent::getUserID()
{
	return m_userID;
}

std::string APFuturesCTPAgent::getBrokerID()
{
	return m_brokerID;
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

#ifdef SIMNOW
	std::string ctpStr = "TestCTP";
#else
	#ifdef CTP_MINI
		std::string ctpStr = "CTPMini1";
	#else
		std::string ctpStr = "StdCTP";
	#endif
#endif
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
