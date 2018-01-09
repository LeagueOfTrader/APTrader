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

	void init();
	void setTradeInstance(APTrade* tradeInstance);

	APTrade* getTradeInstance();

	bool inited();

protected:
	APTrade* m_trader;
	bool m_inited;
};

