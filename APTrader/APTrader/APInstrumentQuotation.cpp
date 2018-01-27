#include "APInstrumentQuotation.h"
#include "APGlobalConfig.h"

APInstrumentQuotation::APInstrumentQuotation(APASSETID instrumentID)
{
	m_instrumentID = instrumentID;
	memset(&m_kLineData, 0, sizeof(m_kLineData));
	m_buyList.resize(5);
	m_sellList.resize(5);
	m_valid = false;
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

APASSETID APInstrumentQuotation::getInstrumentID()
{
	return m_instrumentID;
}

double APInstrumentQuotation::getBuyPrice(UINT index)
{
	if (index >= m_buyList.size()) {
		return 0.0f;
	}

	return m_buyList[index].price;
}

double APInstrumentQuotation::getSellPrice(UINT index)
{
	if (index >= m_sellList.size()) {
		return 0.0f;
	}

	return m_sellList[index].price;
}

long APInstrumentQuotation::getBuyVolume(UINT index)
{
	if (index >= m_buyList.size()) {
		return 0;
	}

	return m_buyList[index].volume;
}

long APInstrumentQuotation::getSellVolume(UINT index)
{
	if (index >= m_sellList.size()) {
		return 0;
	}

	return m_sellList[index].volume;
}

bool APInstrumentQuotation::isValid()
{
	bool ret = false;
	m_mutex.lock();
	ret = m_valid;
	m_mutex.unlock();
	return ret;
}
