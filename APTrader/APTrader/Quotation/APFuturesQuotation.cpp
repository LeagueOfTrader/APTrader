#include "../APMacro.h"
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
	CThostFtdcDepthMarketDataField md;
	memset(&md, 0, sizeof(md));
	bool ret = APFuturesCTPMDAgent::getInstance()->getMarketData(m_instrumentID, md);
	if (ret) {
		m_mutex.lock();
		parseCTPData(md);
		m_valid = true;
		m_mutex.unlock();
	}
#endif
}

#ifdef USE_CTP
void APFuturesQuotation::parseCTPData(const CThostFtdcDepthMarketDataField& data)
{
	if (data.UpdateMillisec == m_timestamp) {
		return;
	}

	m_curPrice = data.LastPrice;
	m_preClosePrice = data.PreClosePrice;
	m_kLineData.highestPrice = data.HighestPrice;
	m_kLineData.lowestPrice = data.LowestPrice;
	m_kLineData.openingPrice = data.OpenPrice;
	m_kLineData.closingPrice = data.ClosePrice;
	m_averagePrice = data.AveragePrice;

	m_buyList[0].price = data.BidPrice1;
	m_buyList[0].volume = data.BidVolume1;
	m_buyList[1].price = data.BidPrice2;
	m_buyList[1].volume = data.BidVolume2;
	m_buyList[2].price = data.BidPrice3;
	m_buyList[2].volume = data.BidVolume3;
	m_buyList[3].price = data.BidPrice4;
	m_buyList[3].volume = data.BidVolume4;
	m_buyList[4].price = data.BidPrice5;
	m_buyList[4].volume = data.BidVolume5;

	m_sellList[0].price = data.AskPrice1;
	m_sellList[0].volume = data.AskVolume1;
	m_sellList[1].price = data.AskPrice2;
	m_sellList[1].volume = data.AskVolume2;
	m_sellList[2].price = data.AskPrice3;
	m_sellList[2].volume = data.AskVolume3;
	m_sellList[3].price = data.AskPrice4;
	m_sellList[3].volume = data.AskVolume4;
	m_sellList[4].price = data.AskPrice5;
	m_sellList[4].volume = data.AskVolume5;

	m_timestamp = data.UpdateMillisec;
}
#endif
