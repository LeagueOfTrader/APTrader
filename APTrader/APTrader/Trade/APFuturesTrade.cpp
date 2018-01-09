#include "APFuturesTrade.h"
#include "../APMacro.h"

#ifdef USE_CTP
#include "../Impl/CTP/APFuturesCTPTraderAgent.h"
#endif

APFuturesTrade::APFuturesTrade()
{
}


APFuturesTrade::~APFuturesTrade()
{
}

void APFuturesTrade::open(APORDERID orderID, APASSETID instrumentID, APTradeDirection direction, double price, long volume, APOrderTimeCondition ot)
{
#ifdef USE_CTP
	APFuturesCTPTraderAgent::getInstance()->applyOrder(TT_Open, instrumentID, price, volume, orderID, direction);
#endif
}

void APFuturesTrade::close(APORDERID orderID, APASSETID instrumentID, APTradeDirection direction, double price, long volume, APOrderTimeCondition ot)
{
#ifdef USE_CTP
	APFuturesCTPTraderAgent::getInstance()->applyOrder(TT_Close, instrumentID, price, volume, orderID, direction);
#endif
}

void APFuturesTrade::open(APASSETID instrumentID, APORDERID localOrderID, APTradeDirection direction, 
							APOrderPriceType orderPriceType, double price, 
							APOrderTimeCondition orderTimeCondition, std::string date, 
							APOrderVolumeCondition orderVolumeCondition, long volume, long minVolume, 
							APOrderContingentCondition orderContingentCondition, double stopPrice)
{
#ifdef USE_CTP
	APFuturesCTPTraderAgent::getInstance()->applyOrder(TT_Open, instrumentID, price, volume, localOrderID, direction, 
														orderPriceType, orderTimeCondition, date, orderVolumeCondition, minVolume, 
														orderContingentCondition, stopPrice);
#endif
}

void APFuturesTrade::close(APASSETID instrumentID, APORDERID localOrderID, APTradeDirection direction, 
							APOrderPriceType orderPriceType, double price, 
							APOrderTimeCondition orderTimeCondition, std::string date, 
							APOrderVolumeCondition orderVolumeCondition, long volume, long minVolume, 
							APOrderContingentCondition orderContingentCondition, double stopPrice)
{
#ifdef USE_CTP
	APFuturesCTPTraderAgent::getInstance()->applyOrder(TT_Close, instrumentID, price, volume, localOrderID, direction,
														orderPriceType, orderTimeCondition, date, orderVolumeCondition, minVolume,
														orderContingentCondition, stopPrice);
#endif
}

void APFuturesTrade::cancel(APORDERID orderID)
{
#ifdef USE_CTP
	APSYSTEMID sysID = getSysIDByOrder(orderID);
	if (sysID.length() > 0) {
		APFuturesCTPTraderAgent::getInstance()->cancelOrder(orderID, sysID);
	}
#endif
}
