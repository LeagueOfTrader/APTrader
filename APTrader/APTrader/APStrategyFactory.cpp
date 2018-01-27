#include "APStrategyFactory.h"
#include "APStrategy.h"
#include "Strategy/APStandardGridStrategy.h"
#include "Strategy/APTransferPositionStrategy.h"

//std::map<std::string, StrategyCreateMethod> APStrategyFactory::m_createMethods;

void APStrategyFactory::init()
{
	//m_createMethods.clear();

	registerCreator("Grid", APStandardGridStrategy::create);
	registerCreator("Transfer", APStandardGridStrategy::create);

	setInited();
}

APStrategy * APStrategyFactory::createStrategy(std::string name)
{
	if (m_createMethods.find(name) != m_createMethods.end()) {
		return m_createMethods[name]();
	}

	return NULL;
}

void APStrategyFactory::registerCreator(std::string name, StrategyCreateMethod function)
{
	m_createMethods[name] = function;
}
