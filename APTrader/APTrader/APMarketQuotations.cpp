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
	std::map<APASSETID, APInstrumentQuotation*>::iterator it = m_instrumentQuotations.find(instrumentID);
	if (it != m_instrumentQuotations.end()) {
		APInstrumentQuotation* quotation = it->second;
		if (quotation != NULL) {
			quotation->incRef();
		}
		return quotation;
	}

	subscribeInstrumentInfo(instrumentID);

	APInstrumentQuotation* instrumentQuotation = this->generateQuotation(instrumentID);//APObjectFactory::newInstrumentQuotation(m_marketType, instrumentID);
	m_instrumentQuotations[instrumentID] = instrumentQuotation;
	return instrumentQuotation;
}

void APMarketQuotations::unSubscribeInstrument(APASSETID instrumentID)
{
	std::map<APASSETID, APInstrumentQuotation*>::iterator it = m_instrumentQuotations.find(instrumentID);
	if (it != m_instrumentQuotations.end()) {
		APInstrumentQuotation* quotation = it->second;
		if (quotation != NULL) {
			quotation->decRef();
		}
	}
}

void APMarketQuotations::init()
{
	m_marketType = APGlobalConfig::getInstance()->getInstrumentType();
}

void APMarketQuotations::update(float deltaTime)
{
	std::map<std::string, APInstrumentQuotation*>::iterator it = m_instrumentQuotations.begin();
	while (it != m_instrumentQuotations.end()) {
		APInstrumentQuotation* quotation = it->second;
		if (quotation != NULL) {
			if (quotation->getRefCount() <= 0) {
				unSubscribeInstrumentInfo(quotation->getInstrumentID());
				m_instrumentQuotations.erase(it++);
				continue;
			}
			quotation->queryQuotation();
		}
		it++;
	}
}

APInstrumentQuotation * APMarketQuotations::generateQuotation(APASSETID instrumentID)
{
	return APObjectFactory::newInstrumentQuotation(m_marketType, instrumentID);
}
