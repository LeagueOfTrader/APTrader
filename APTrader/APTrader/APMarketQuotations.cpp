#include "APMarketQuotations.h"
#include "APInstrumentQuotation.h"
#include "APObjectFactory.h"
#include "APGlobalConfig.h"


APMarketQuotations::APMarketQuotations()
{
	//
}


APMarketQuotations::~APMarketQuotations()
{
}

APInstrumentQuotation* APMarketQuotations::subscribeInstrument(APASSETID instrumentID)
{
	std::map<std::string, APInstrumentQuotation*>::iterator it = m_instrumentQuotations.find(instrumentID);
	if (it != m_instrumentQuotations.end()) {
		return  it->second;
	}

	APInstrumentQuotation* instrumentQuotation = this->generateQuotation(instrumentID);//APObjectFactory::newInstrumentQuotation(m_marketType, instrumentID);
	m_instrumentQuotations[instrumentID] = instrumentQuotation;
	return instrumentQuotation;
}

void APMarketQuotations::init()
{
	m_marketType = APGlobalConfig::getInstance()->getInstrumentType();
}

void APMarketQuotations::update()
{
	std::map<std::string, APInstrumentQuotation*>::iterator it = m_instrumentQuotations.begin();
	for (; it != m_instrumentQuotations.end(); it++) {
		it->second->queryQuotation();
	}
}

APInstrumentQuotation * APMarketQuotations::generateQuotation(APASSETID instrumentID)
{
	return APObjectFactory::newInstrumentQuotation(m_marketType, instrumentID);
}
