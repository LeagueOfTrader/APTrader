#include "APFuturesTrade.h"
#include "../APMarco.h"

#ifdef USE_CTP
#include "../Impl/APFuturesCTPTraderAgent.h"
#endif

APFuturesTrade::APFuturesTrade()
{
}


APFuturesTrade::~APFuturesTrade()
{
}

void APFuturesTrade::open(APORDERID orderID, APASSETID instrumentID, APTrendType trend, double price, long volume, APOrderTimeCondition ot)
{
#ifdef USE_CTP
	APFuturesCTPTraderAgent::getInstance()->applyOrder(TDT_Open, instrumentID, price, volume, orderID, trend);
#endif
}

void APFuturesTrade::close(APORDERID orderID, APASSETID instrumentID, APTrendType trend, double price, long volume, APOrderTimeCondition ot)
{
#ifdef USE_CTP
	APFuturesCTPTraderAgent::getInstance()->applyOrder(TDT_Close, instrumentID, price, volume, orderID, trend);
#endif
}

void APFuturesTrade::open(APASSETID instrumentID, APORDERID localOrderID, APTrendType trend, 
							APOrderPriceType orderPriceType, double price, 
							APOrderTimeCondition orderTimeCondition, std::string date, 
							APOrderVolumeCondition orderVolumeCondition, long volume, long minVolume, 
							APOrderContingentCondition orderContingentCondition, double stopPrice)
{
#ifdef USE_CTP
	APFuturesCTPTraderAgent::getInstance()->applyOrder(TDT_Open, instrumentID, price, volume, localOrderID, trend, 
														orderPriceType, orderTimeCondition, date, orderVolumeCondition, minVolume, 
														orderContingentCondition, stopPrice);
#endif
}

void APFuturesTrade::close(APASSETID instrumentID, APORDERID localOrderID, APTrendType trend, 
							APOrderPriceType orderPriceType, double price, 
							APOrderTimeCondition orderTimeCondition, std::string date, 
							APOrderVolumeCondition orderVolumeCondition, long volume, long minVolume, 
							APOrderContingentCondition orderContingentCondition, double stopPrice)
{
#ifdef USE_CTP
	APFuturesCTPTraderAgent::getInstance()->applyOrder(TDT_Close, instrumentID, price, volume, localOrderID, trend,
														orderPriceType, orderTimeCondition, date, orderVolumeCondition, minVolume,
														orderContingentCondition, stopPrice);
#endif
}

void APFuturesTrade::cancel(APORDERID orderID)
{
#ifdef USE_CTP
	APSYSTEMID sysID = getSysIDByOrder(orderID);
	if (sysID > 0) {
		APFuturesCTPTraderAgent::getInstance()->cancelOrder(orderID, sysID);
	}
#endif
}
