#include "APFuturesCTPMDAgent.h"

#ifdef USE_CTP

#include "../Utils/APLog.h"
#include "APFuturesCTPMDResponser.h"

//
////test
//#include "../Quotation/APFuturesMarketQuotations.h"
//

APFuturesCTPMDResponser::APFuturesCTPMDResponser()
{
}


APFuturesCTPMDResponser::~APFuturesCTPMDResponser()
{
}

void APFuturesCTPMDResponser::OnFrontConnected()
{
	APFuturesCTPMDAgent::getInstance()->login();
}

void APFuturesCTPMDResponser::OnFrontDisconnected(int nReason)
{
	APLogger->log("CTP MD Front Disconnected, error: %d. ", nReason);
}

void APFuturesCTPMDResponser::OnHeartBeatWarning(int nTimeLapse)
{
}

void APFuturesCTPMDResponser::OnRspUserLogin(CThostFtdcRspUserLoginField * pRspUserLogin, CThostFtdcRspInfoField * pRspInfo, int nRequestID, bool bIsLast)
{
	APFuturesCTPMDAgent::getInstance()->onLogin();
}

void APFuturesCTPMDResponser::OnRspUserLogout(CThostFtdcUserLogoutField * pUserLogout, CThostFtdcRspInfoField * pRspInfo, int nRequestID, bool bIsLast)
{
}

void APFuturesCTPMDResponser::OnRspError(CThostFtdcRspInfoField * pRspInfo, int nRequestID, bool bIsLast)
{
	APLogger->log(pRspInfo->ErrorMsg);
}

void APFuturesCTPMDResponser::OnRspSubMarketData(CThostFtdcSpecificInstrumentField * pSpecificInstrument, CThostFtdcRspInfoField * pRspInfo, int nRequestID, bool bIsLast)
{
	APLogger->log("On Subscribe Market Data, Instrument ID: %s, ErrorID: %d, ErrorMsg: %s\n", 
				pSpecificInstrument->InstrumentID, pRspInfo->ErrorID, pRspInfo->ErrorMsg);
}

void APFuturesCTPMDResponser::OnRspUnSubMarketData(CThostFtdcSpecificInstrumentField * pSpecificInstrument, CThostFtdcRspInfoField * pRspInfo, int nRequestID, bool bIsLast)
{
	APLogger->log("On UnSubscribe Market Data, Instrument ID: %s, ErrorID: %d, ErrorMsg: %s\n",
		pSpecificInstrument->InstrumentID, pRspInfo->ErrorID, pRspInfo->ErrorMsg);
}

void APFuturesCTPMDResponser::OnRspSubForQuoteRsp(CThostFtdcSpecificInstrumentField * pSpecificInstrument, CThostFtdcRspInfoField * pRspInfo, int nRequestID, bool bIsLast)
{
}

void APFuturesCTPMDResponser::OnRspUnSubForQuoteRsp(CThostFtdcSpecificInstrumentField * pSpecificInstrument, CThostFtdcRspInfoField * pRspInfo, int nRequestID, bool bIsLast)
{
}

void APFuturesCTPMDResponser::OnRtnDepthMarketData(CThostFtdcDepthMarketDataField * pDepthMarketData)
{	
	APFuturesCTPMDAgent::getInstance()->onGetMarketData(pDepthMarketData);
	
	//APLogger->log("On Get Market Data, Instrument ID: %s, Price: %f. ",
	//				pDepthMarketData->InstrumentID, pDepthMarketData->LastPrice);
}

void APFuturesCTPMDResponser::OnRtnForQuoteRsp(CThostFtdcForQuoteRspField * pForQuoteRsp)
{
}

bool APFuturesCTPMDResponser::isErrorRspInfo(CThostFtdcRspInfoField * pRspInfo)
{
	bool bResult = ((pRspInfo) && (pRspInfo->ErrorID != 0));
	if (bResult) {
		APLogger->log("Rsp Info Error, ErrorID: %d, ErrorMsg: %s. ", pRspInfo->ErrorID, pRspInfo->ErrorMsg);
	}

	return bResult;
}


#endif