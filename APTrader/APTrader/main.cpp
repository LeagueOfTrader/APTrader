#pragma once
#include "APMacro.h"
#include "Strategy/APStandardGridStrategy.h"
#include "APFramework.h"
#include "APStrategyManager.h"
#include "APStrategyFactory.h"
#include <windows.h>
#include "APGlobalConfig.h"
#include "Utils/APJsonReader.h"
//#include "Simulation/APSimFramework.h"
#include "Impl/CTP/APFuturesCTPMDAgent.h"
#include "Quotation/APFuturesMarketQuotations.h"
#include "Monitor/APMonitorFramework.h"
#include <thread>
#include "Utils/APRedisAgent.h"
//
#include "Test/APTestFramework.h"
#include "APTradeManager.h"
#include "APTrade.h"
#include "PositionCtrl/APFuturesPositionCtrl.h"
#include "Impl/CTP/APFuturesCTPTraderAgent.h"

void runMonitor() {
	//// ---- monitor ----
	APMonitorFramework* framework = APMonitorFramework::getInstance();
	//std::thread frameworkThread(frameworkLoop);
	framework->init();

	while (!framework->inited()) {
		Sleep(10);
	}

	while (!framework->finished()) {
		framework->update();
	}

	framework->exit();

	delete framework;
	//// ---- monitor ----
}

void runSimulation() {
	//APSimFramework* framework = new APSimFramework();
	//if (framework != NULL)
	//{
	//	framework->init();

	//	//APStandardGridStrategy* gs = (APStandardGridStrategy*)APStrategyManager::getInstance()->createStrategy("Data/grid0.stg");
	//	//gs->printGrids();
	//	
	//	while (!framework->finished()) {
	//		framework->update();
	//		Sleep(300);
	//	}
	//}
	//framework->exit();
	//delete framework;
}

void runTest() {
	APTestFramework* framework = APTestFramework::getInstance();
	framework->init();
	while (!framework->inited()) {
		Sleep(1000);
	}

	APTradeManager::getInstance()->init();

	APTrade* trader = APTradeManager::getInstance()->getTradeInstance();
	APFuturesPositionCtrl* posCtrl = new APFuturesPositionCtrl();
	std::string instID = "rb1805";
	APTradeDirection dir = TD_Sell;
	posCtrl->setInstrumentID(instID);
	posCtrl->setTradeDirection(dir);
	posCtrl->setTrade(trader);

	//posCtrl->open(instID, dir, 4000.0, 10);

	APFuturesCTPTraderAgent::getInstance()->reqQryAllInvestorPosition();
	//APFuturesCTPTraderAgent::getInstance()->reqQryAllInvestorPositionDetail();
	//APFuturesCTPTraderAgent::getInstance()->reqQryTradingAccount();
	//APFuturesCTPTraderAgent::getInstance()->reqQryAllOrders();

	while (true) {
		framework->update();
		Sleep(1000);
	}
}

void main() {
	runTest();
}



