#include "APFuturesQuotationCollector.h"
#ifdef USE_CTP
#include "../../Impl/CTP/APFuturesCTPMDAgent.h"
#endif

#include "APFuturesDataCollector.h"

APFuturesQuotationCollector::APFuturesQuotationCollector(APASSETID instrumentID) : APFuturesQuotation(instrumentID)
{
}


APFuturesQuotationCollector::~APFuturesQuotationCollector()
{
}

void APFuturesQuotationCollector::queryQuotation()
{
#ifdef USE_CTP
	CThostFtdcDepthMarketDataField md;
	memset(&md, 0, sizeof(md));
	bool ret = APFuturesCTPMDAgent::getInstance()->getMarketData(m_instrumentID, md);
	if (ret) {
		//m_mutex.lock();
		//parseCTPData(md);
		APFuturesDataCollector::getInstance()->collect(m_instrumentID, md);
		m_valid = true;
		//m_mutex.unlock();
	}
#endif
}
