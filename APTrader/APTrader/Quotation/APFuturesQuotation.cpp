#include "../APMarco.h"
#include "APFuturesQuotation.h"

#ifdef USE_CTP
#include "../Impl/CTP/APFuturesCTPMDAgent.h"
#endif

APFuturesQuotation::APFuturesQuotation(APASSETID instrumentID) : APInstrumentQuotation(instrumentID)
{
#ifdef USE_CTP
	m_timestamp = 0;
#endif
}


APFuturesQuotation::~APFuturesQuotation()
{
}

void APFuturesQuotation::queryQuotation()
{
#ifdef USE_CTP
	CThostFtdcDepthMarketDataField* data = APFuturesCTPMDAgent::getInstance()->getMarketData(m_instrumentID);
	if (data != NULL) {
		parseCTPData(data);
		m_valid = true;
	}
#endif
}

#ifdef USE_CTP
void APFuturesQuotation::parseCTPData(CThostFtdcDepthMarketDataField * data)
{
	if (data == NULL) {
		return;
	}

	if (data->UpdateMillisec == m_timestamp) {
		return;
	}

	m_curPrice = data->LastPrice;
	m_preClosePrice = data->PreClosePrice;
	m_kLineData.highestPrice = data->HighestPrice;
	m_kLineData.lowestPrice = data->LowestPrice;
	m_kLineData.openingPrice = data->OpenPrice;
	m_kLineData.closingPrice = data->ClosePrice;
	m_averagePrice = data->AveragePrice;

	m_openOrderList[0].price = data->BidPrice1;
	m_openOrderList[0].volume = data->BidVolume1;
	m_openOrderList[1].price = data->BidPrice2;
	m_openOrderList[1].volume = data->BidVolume2;
	m_openOrderList[2].price = data->BidPrice3;
	m_openOrderList[2].volume = data->BidVolume3;
	m_openOrderList[3].price = data->BidPrice4;
	m_openOrderList[3].volume = data->BidVolume4;
	m_openOrderList[4].price = data->BidPrice5;
	m_openOrderList[4].volume = data->BidVolume5;

	m_closeOrderList[0].price = data->AskPrice1;
	m_closeOrderList[0].volume = data->AskVolume1;
	m_closeOrderList[1].price = data->AskPrice2;
	m_closeOrderList[1].volume = data->AskVolume2;
	m_closeOrderList[2].price = data->AskPrice3;
	m_closeOrderList[2].volume = data->AskVolume3;
	m_closeOrderList[3].price = data->AskPrice4;
	m_closeOrderList[3].volume = data->AskVolume4;
	m_closeOrderList[4].price = data->AskPrice5;
	m_closeOrderList[4].volume = data->AskVolume5;

	m_timestamp = data->UpdateMillisec;
}
#endif
