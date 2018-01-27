#include "APGlobalConfig.h"
#include "Utils/APJsonReader.h"

std::string globalConfigFile = "Data/System/GlobalConfig.cfg";

APGlobalConfig::APGlobalConfig()
{	
	m_autoCorrectPosition = false;
	m_useRepertory = false;
	m_counteractPosition = true;
	m_manualPosition = false;
	m_autoRunStrategy = true;
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

	if (jr.hasMember("AutoCorrectPosition")) {
		m_autoCorrectPosition = jr.getBoolValue("AutoCorrectPosition");
	}

	if (jr.hasMember("UseRepertory")) {
		m_useRepertory = jr.getBoolValue("UseRepertory");
	}

	if (jr.hasMember("CounteractPosition")) {
		m_counteractPosition = jr.getBoolValue("CounteractPosition");
	}

	if (jr.hasMember("ManualPosition")) {
		m_manualPosition = jr.getBoolValue("ManualPosition");
	}

	if (jr.hasMember("AutoRunStrategy")) {
		m_autoRunStrategy = jr.getBoolValue("AutoRunStrategy");
	}

	m_openLimit = OPL_Position;

	setInited();
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

bool APGlobalConfig::isAutoCorrectPosition()
{
	return m_autoCorrectPosition;
}

bool APGlobalConfig::useRepertory()
{
	return m_useRepertory;
}

bool APGlobalConfig::counteractPosition()
{
	return m_counteractPosition;
}

bool APGlobalConfig::isManualPosition()
{
	return m_manualPosition;
}

bool APGlobalConfig::isAutoRunStrategy()
{
	return m_autoRunStrategy;
}
