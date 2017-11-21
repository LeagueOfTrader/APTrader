#include "APSimAccount.h"
#include "../Utils/APJsonReader.h"
#include "../APPositionManager.h"
#include "../APTradeManager.h"
#include "APSimTradeSystem.h"
#include "../Utils/APLog.h"
//#include "APSimFuturesTrade.h"

std::string simAccountFile = "Data/Sim/Account.cfg";

APSimAccount::APSimAccount()
{
}


APSimAccount::~APSimAccount()
{
}

void APSimAccount::init()
{
	APJsonReader jr;
	jr.initWithFile(simAccountFile);

	m_fund = jr.getDoubleValue("Money");
}

double APSimAccount::getFund()
{
	return m_fund;
}

bool APSimAccount::takeFund(double fund)
{
	if (fund > m_fund) {
		return false;
	}

	m_fund -= fund;
	return true;
}

bool APSimAccount::saveFund(double fund)
{
	m_fund += fund;
	APLogger->log("**** Sim Account Fund: %.2f. (%.2f)", m_fund, fund);
	return true;
}

double APSimAccount::getTotalAssetsValue()
{
	double assetsProfitAndLoss = calcAssetsProfitAndLoss();
	return m_fund + assetsProfitAndLoss;
}

double APSimAccount::calcAssetsProfitAndLoss()
{
	double assetsFloating = APSimTradeSystem::getInstance()->calcFloatingProfit();
	
	return assetsFloating;
}
