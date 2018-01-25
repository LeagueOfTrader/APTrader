#pragma once
#include "Common/Singleton.h"
#include "Common/InitializableObject.h"
#include <string>
#include "APDef.h"

class APTrade;
class APPositionCtrl;

class APTradeManager : public InitializableObject, public Singleton<APTradeManager>
{
public:
	APTradeManager();
	~APTradeManager();

	void init();
	void exit();
	void setTradeInstance(APTrade* tradeInstance);

	APTrade* getTradeInstance();

protected:
	APTrade* m_trader;
};

