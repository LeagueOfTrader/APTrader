#include "APSystemSetting.h"
#include "Utils/APJsonReader.h"
#include "3rdParty/jsoncpp/include/json/writer.h"

const std::string systemSettingKey = "SystemSetting";
const long DEFAULT_STRATEGY_QUERY_INTERVAL = 100;
const long DEFAULT_INIT_STATE_WAIT_INTERVAL = 10;
const long DEFAULT_MD_QUERY_INTERVAL = 500;
const long DEFAULT_HOLD_POSITION_QUERY_INTERVAL = 1000;

APSystemSetting::APSystemSetting()
{
}

APSystemSetting::~APSystemSetting()
{
}

void APSystemSetting::init()
{
	if (!load()) {
		// read default params
		m_strategyUpdateInterval = DEFAULT_STRATEGY_QUERY_INTERVAL;
		m_initializeStateWaitInterval = DEFAULT_INIT_STATE_WAIT_INTERVAL;
		m_marketDataQueryInterval = DEFAULT_MD_QUERY_INTERVAL;
		m_holdPositionQueryInterval = DEFAULT_HOLD_POSITION_QUERY_INTERVAL;
	}
}

long APSystemSetting::getStrategyUpdateInterval()
{
	return m_strategyUpdateInterval;
}

long APSystemSetting::getInitializeStateWaitInterval()
{
	return m_initializeStateWaitInterval;
}

long APSystemSetting::getMarketDataQueryInterval()
{
	return m_marketDataQueryInterval;
}

long APSystemSetting::getHoldPositionQueryInterval()
{
	return m_holdPositionQueryInterval;
}

std::string APSystemSetting::serialize()
{
	Json::Value v;
	v["StrategyInterval"] = m_strategyUpdateInterval;
	v["InitStateInterval"] = m_initializeStateWaitInterval;
	v["MarketDataInterval"] = m_marketDataQueryInterval;
	v["HoldPositionInterval"] = m_holdPositionQueryInterval;

	Json::FastWriter fw;
	return fw.write(v);;
}

void APSystemSetting::deserialize(std::string str)
{
	Json::Value v;
	Json::Reader jr;
	jr.parse(str, v);
	m_strategyUpdateInterval = v["StrategyInterval"].asUInt();
	m_initializeStateWaitInterval = v["InitStateInterval"].asUInt();
	m_marketDataQueryInterval = v["MarketDataInterval"].asUInt();
	m_holdPositionQueryInterval = v["HoldPositionInterval"].asUInt();
}
