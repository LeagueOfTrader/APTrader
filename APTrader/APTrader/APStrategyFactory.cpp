#include "APStrategyFactory.h"
#include "APStrategy.h"
#include "Strategy/APStandardGridStrategy.h"
#include "Strategy/APTransferPositionStrategy.h"

std::map<std::string, StrategyCreateMethod> APStrategyFactory::m_createMethods;

void APStrategyFactory::init()
{
	m_createMethods.clear();

	m_createMethods["Grid"] = APStandardGridStrategy::create;
	m_createMethods["Transfer"] = APTransferPositionStrategy::create;
}

APStrategy * APStrategyFactory::createStrategy(std::string name)
{
	if (m_createMethods.find(name) != m_createMethods.end()) {
		return m_createMethods[name]();
	}

	return NULL;
}
