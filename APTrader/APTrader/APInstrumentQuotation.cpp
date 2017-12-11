#include "APInstrumentQuotation.h"
#include "APGlobalConfig.h"

APInstrumentQuotation::APInstrumentQuotation(APASSETID instrumentID)
{
	m_instrumentID = instrumentID;
}


APInstrumentQuotation::~APInstrumentQuotation()
{
}

double APInstrumentQuotation::getCurPrice()
{
	return m_curPrice;
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
