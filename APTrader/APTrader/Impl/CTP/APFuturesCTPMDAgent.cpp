#include "APFuturesCTPMDAgent.h"
#include "../../Utils/APJsonReader.h"

std::string mdflowpath = "Data/CTP/mdflow/";

APFuturesCTPMDAgent::APFuturesCTPMDAgent()
{
}


APFuturesCTPMDAgent::~APFuturesCTPMDAgent()
{
}

void APFuturesCTPMDAgent::initCTP() 
{
	m_mdApi = CThostFtdcMdApi::CreateFtdcMdApi(mdflowpath.c_str(), true);
	m_mdResponser = new APFuturesCTPMDResponser();	
	m_mdApi->RegisterSpi(m_mdResponser);
	m_mdApi->RegisterFront((char*)m_marketFront.c_str());
	m_mdApi->Init();
	//m_mdApi->Join();
}

void APFuturesCTPMDAgent::login() 
{
	CThostFtdcReqUserLoginField req;
	memset(&req, 0, sizeof(req));
	strcpy(req.BrokerID, m_brokerID.c_str());
	strcpy(req.UserID, m_userID.c_str());
	strcpy(req.Password, m_password.c_str());

	int ret = m_mdApi->ReqUserLogin(&req, genReqID());
}

CThostFtdcMdApi* APFuturesCTPMDAgent::getMDApi() {
	return m_mdApi;
}

bool APFuturesCTPMDAgent::subscribeInstrument(APASSETID instrumentID)
{
	if (m_mdApi == NULL) {
		return false;
	}

	if (m_subscribedInstruments.find(instrumentID) != m_subscribedInstruments.end()) {
		return true;
	}

	char** ppArr = new char*[1];
	ppArr[0] = new char[32];
	strcpy(ppArr[0], instrumentID.c_str());
	int ret = m_mdApi->SubscribeMarketData(ppArr, 1);
	delete[] ppArr[0];
	delete[] ppArr;

	if (ret) {
		m_subscribedInstruments.insert(instrumentID);
	}

	//int count = m_subscribedInstruments.size();
	//char** ppSubscribeArr = new char*[count];

	//std::set<APASSETID>::iterator it;

	//int i = 0;
	//for (it = m_subscribedInstruments.begin(); it != m_subscribedInstruments.end(); it++) {
	//	APASSETID id = *it;
	//	ppSubscribeArr[i] = new char[32];
	//	strcpy(ppSubscribeArr[i], id.c_str());
	//	i++;
	//}

	//bool ret = m_mdApi->SubscribeMarketData(ppSubscribeArr, m_subscribedInstruments.size());

	//for (i = 0; i < count; i++) {
	//	delete[] ppSubscribeArr[i];
	//}
	//delete[] ppSubscribeArr;

	return ret;
}

bool APFuturesCTPMDAgent::unSubscribeInstrument(APASSETID instrumentID)
{
	if (m_mdApi == NULL) {
		return false;
	}

	if (m_subscribedInstruments.find(instrumentID) == m_subscribedInstruments.end()) {
		return false;
	}
	
	char** ppArr = new char*[1];
	ppArr[0] = new char[32];
	strcpy(ppArr[0], instrumentID.c_str());
	int ret = m_mdApi->UnSubscribeMarketData(ppArr, 1);
	delete[] ppArr[0];
	delete[] ppArr;

	if (ret) {
		m_subscribedInstruments.erase(instrumentID);
	}

	return ret;
}

bool APFuturesCTPMDAgent::getMarketData(APASSETID instrumentID, CThostFtdcDepthMarketDataField& data)
{
	if(m_marketData.find(instrumentID) == m_marketData.end()){
		return false;
	}

	data = m_marketData[instrumentID];
	return true;
}

void APFuturesCTPMDAgent::onGetMarketData(CThostFtdcDepthMarketDataField * data)
{
	if (data == NULL) {
		return;
	}

	APASSETID instrumentID = data->InstrumentID;

	m_marketData[instrumentID] = *data;
}
