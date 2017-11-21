#include "APMarketQuotations.h"
#include "APCommodityQuotation.h"
#include "APObjectFactory.h"
#include "APGlobalConfig.h"


APMarketQuotations::APMarketQuotations()
{
	//
}


APMarketQuotations::~APMarketQuotations()
{
}

APCommodityQuotation* APMarketQuotations::subscribeCommodity(APASSETID commodityID)
{
	std::map<std::string, APCommodityQuotation*>::iterator it = m_commodityQuotations.find(commodityID);
	if (it != m_commodityQuotations.end()) {
		return  it->second;
	}

	APCommodityQuotation* commodityQuotation = this->generateQuotation(commodityID);//APObjectFactory::newCommodityQuotation(m_marketType, commodityID);
	m_commodityQuotations[commodityID] = commodityQuotation;
	return commodityQuotation;
}

void APMarketQuotations::init()
{
	m_marketType = APGlobalConfig::getInstance()->getCommodityType();
}

void APMarketQuotations::update()
{
	std::map<std::string, APCommodityQuotation*>::iterator it = m_commodityQuotations.begin();
	for (; it != m_commodityQuotations.end(); it++) {
		it->second->queryQuotation();
	}
}

APCommodityQuotation * APMarketQuotations::generateQuotation(APASSETID commodityID)
{
	return APObjectFactory::newCommodityQuotation(m_marketType, commodityID);
}
