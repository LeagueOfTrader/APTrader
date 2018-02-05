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

#include "Futures/APFuturesFramework.h"
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

//void inputThreadLoop() {
//	char cmd[64];
//	while (scanf("%s", &cmd) != EOF) {
//		if (strcmp(cmd, "quit") == 0) {
//			APTestFramework::getInstance()->terminate();
//		}
//	}
//}

void runTest() {
	//std::thread inputThread(inputThreadLoop);
	
	APTestFramework* framework = APTestFramework::getInstance();

	framework->init();
	while (!framework->inited()) {
		Sleep(1000);
	}

	//APTradeManager::getInstance()->init();

	//APTrade* trader = APTradeManager::getInstance()->getTradeInstance();
	//trader->init();

	//APFuturesPositionCtrl* posCtrl = new APFuturesPositionCtrl();
	//std::string instID = "j1805";
	//APTradeDirection dir = TD_Sell;
	//posCtrl->setInstrumentID(instID);
	//posCtrl->setTradeDirection(dir);
	//posCtrl->setTrade(trader);

	////posCtrl->open(instID, dir, 2030.0, 10);

	////APFuturesCTPTraderAgent::getInstance()->reqQryAllInvestorPosition();
	////APFuturesCTPTraderAgent::getInstance()->reqQryAllInvestorPositionDetail();
	////APFuturesCTPTraderAgent::getInstance()->reqQryTradingAccount();
	////APFuturesCTPTraderAgent::getInstance()->reqQryAllOrders();

	while (!framework->finished()) {
		framework->update();
		Sleep(1000);
	}

	//inputThread.detach();
}

void runFuturesFramework() {
	APFuturesFramework* framework = APFuturesFramework::getInstance();
	framework->ready();
	framework->start();

	while (!framework->isCTPInited())
	{
		Sleep(100);
	}

	framework->initLocalSystem();

	while (!framework->inited()) {
		Sleep(100);
	}

	while (!framework->finished()) {
		framework->update();
		Sleep(10);
	}

	framework->exit();
}



void main() {
	runFuturesFramework();
	
	//runTest();
}



