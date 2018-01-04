#pragma once
#include "Common/Singleton.h"
#include <string>
#include "APDef.h"

class APTrade;
class APPositionCtrl;

class APTradeManager : public Singleton<APTradeManager>
{
public:
	APTradeManager();
	~APTradeManager();

	//void registerPositionCtrl(APASSETID instrumentID, APTradeDirection direction, APPositionCtrl* positionCtrl);
	//void unregisterPositionCtrl(APPositionCtrl* positionCtrl);

	void init();
	void setTradeInstance(APTrade* tradeInstance);

	APTrade* getTradeInstance();

protected:
	APTrade* m_trader;
};

