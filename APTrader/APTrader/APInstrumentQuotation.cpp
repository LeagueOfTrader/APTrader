#include "APInstrumentQuotation.h"
#include "APGlobalConfig.h"

APInstrumentQuotation::APInstrumentQuotation(APASSETID instrumentID)
{
	m_instrumentID = instrumentID;
	memset(&m_kLineData, 0, sizeof(m_kLineData));
	m_openOrderList.resize(5);
	m_closeOrderList.resize(5);
}


APInstrumentQuotation::~APInstrumentQuotation()
{
}

double APInstrumentQuotation::getCurPrice()
{
	return m_curPrice;
}

double APInstrumentQuotation::getPreClosePrice()
{
	return m_preClosePrice;
}

APKLine & APInstrumentQuotation::getKLineData()
{
	return m_kLineData;
}

double APInstrumentQuotation::getAveragePrice()
{
	return m_averagePrice;
}

double APInstrumentQuotation::getOpenPrice(UINT index)
{
	if (index >= m_openOrderList.size()) {
		return 0.0f;
	}

	return m_openOrderList[index].price;
}

double APInstrumentQuotation::getClosePrice(UINT index)
{
	if (index >= m_closeOrderList.size()) {
		return 0.0f;
	}

	return m_closeOrderList[index].price;
}

long APInstrumentQuotation::getOpenVolume(UINT index)
{
	if (index >= m_openOrderList.size()) {
		return 0;
	}

	return m_openOrderList[index].volume;
}

long APInstrumentQuotation::getCloseVolume(UINT index)
{
	if (index >= m_closeOrderList.size()) {
		return 0;
	}

	return m_closeOrderList[index].volume;
}
