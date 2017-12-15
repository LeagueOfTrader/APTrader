#pragma once
#include "../APFramework.h"
#include <vector>

class APTrade;

enum APSimMode {
	SM_Continuous,
	SM_Discrete,
	SM_Num
};

class APSimFramework :
	public APFramework
{
public:
	APSimFramework();
	~APSimFramework();

	virtual void init();
	virtual void update(float deltaTime);
	virtual void exit();

private:
	void initFromCfg();
	void postInit();
	APTrade* generateSimTradeInstance();

protected:
	APSimMode m_mode;
	int m_discreteTimes;
	int m_discreteCount;

	std::vector<std::string> m_simStrategies;
};

