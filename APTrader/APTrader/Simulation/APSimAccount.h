#pragma once
#include "../Common/Singleton.h"

class APSimAccount : public Singleton<APSimAccount>
{
public:
	APSimAccount();
	~APSimAccount();
	
	void init();

	double getFund();
	bool takeFund(double fund);
	bool saveFund(double fund);

	double getTotalAssetsValue();
	double calcAssetsProfitAndLoss();

private:
	double m_fund;
};

