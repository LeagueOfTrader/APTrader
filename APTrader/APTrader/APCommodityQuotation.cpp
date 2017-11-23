#include "APCommodityQuotation.h"
#include "APGlobalConfig.h"

APCommodityQuotation::APCommodityQuotation(APASSETID commodityID)
{
	m_commodityID = commodityID;
}


APCommodityQuotation::~APCommodityQuotation()
{
}

double APCommodityQuotation::getCurPrice()
{
	return m_curPrice;
}

double APCommodityQuotation::getOpenPrice(UINT index)
{
	if (index >= m_openOrderList.size()) {
		return 0.0f;
	}

	return m_openOrderList[index].price;
}

double APCommodityQuotation::getClosePrice(UINT index)
{
	if (index >= m_closeOrderList.size()) {
		return 0.0f;
	}

	return m_closeOrderList[index].price;
}

long APCommodityQuotation::getOpenVolume(UINT index)
{
	if (index >= m_openOrderList.size()) {
		return 0;
	}

	return m_openOrderList[index].volume;
}

long APCommodityQuotation::getCloseVolume(UINT index)
{
	if (index >= m_closeOrderList.size()) {
		return 0;
	}

	return m_closeOrderList[index].volume;
}
