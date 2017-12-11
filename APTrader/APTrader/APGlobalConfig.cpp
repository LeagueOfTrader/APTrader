#include "APGlobalConfig.h"
#include "Utils/APJsonReader.h"

std::string globalConfigFile = "Data/System/GlobalConfig.cfg";

APGlobalConfig::APGlobalConfig()
{	
}

APGlobalConfig::~APGlobalConfig()
{
}

void APGlobalConfig::init()
{
	APJsonReader jr;
	jr.initWithFile(globalConfigFile);
	std::string type = jr.getStrValue("TradeType");
	if (type == "Futures") {
		m_financialInstrumentType = FCT_Futrues;
	}
	else if (type == "Shares") {
		m_financialInstrumentType = FCT_Shares;
	}

	m_nearByPrice = jr.getDoubleValue("NearbyPrice");

	m_visibleOrdersCount = 5; // sync VIP param

	//std::string openLimit = jr.getStrValue("OpenLimit");
	//if (openLimit == "Fund") {
	//	m_openLimit = OPL_Fund;
	//}
	//else if (openLimit == "Position") {
	//	m_openLimit = OPL_Position;
	//}
	m_openLimit = OPL_Position;
}

APFinancialInstrumentType APGlobalConfig::getInstrumentType()
{
	return m_financialInstrumentType;
}

UINT APGlobalConfig::getTransactionDays()
{
	UINT tPlusNDays = 0;
	switch (m_financialInstrumentType)
	{
	case FCT_Futrues:
		break;
	case FCT_Shares:
		tPlusNDays = 1;
		break;
	case FCT_Options:
		break;
	default:
		break;
	}

	return tPlusNDays;
}

double APGlobalConfig::getNearByPrice()
{
	return m_nearByPrice;
}

UINT APGlobalConfig::getVisibleOrdersCount()
{
	return m_visibleOrdersCount;
}

APOpenPositionLimit APGlobalConfig::getOpenLimitType()
{
	return m_openLimit;
}
