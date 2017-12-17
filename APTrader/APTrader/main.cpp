#include "APMarco.h"
#include "Strategy/APStandardGridStrategy.h"
#include "APFramework.h"
#include "APStrategyManager.h"
#include "APStrategyFactory.h"
#include <windows.h>
#include "APGlobalConfig.h"
#include "Utils/APJsonReader.h"
#include "Simulation/APSimFramework.h"
#include "Impl/APFuturesCTPMDAgent.h"
#include "Quotation/APFuturesMarketQuotations.h"

void loop() {
	return;

	std::string instrumentID = "hc1805";
	//APFuturesMarketQuotations::getInstance()->init();
	
	DWORD lastTickCount = GetTickCount();
	while (true) {
		DWORD curTickCount = GetTickCount();
		float deltaTime = (float)(curTickCount - lastTickCount) / 1000.0f;
		lastTickCount = curTickCount;
		APFuturesCTPMDAgent::getInstance()->subscribeInstrument(instrumentID);
	}
}

void main() {

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
	APFuturesCTPMDAgent::getInstance()->registerInitCallback(loop);
	APFuturesCTPMDAgent::getInstance()->init();
	
	//DWORD lastTickCount = GetTickCount();
	//while (1) {
	//	// wait
	//	//Sleep(100);
	//	DWORD curTickCount = GetTickCount();
	//	float deltaTime = (float)(curTickCount - lastTickCount) / 1000.0f;
	//	// update(deltaTime);
	//}
	//
	//std::string instrumentID = "hc1805";
	//APFuturesMarketQuotations::getInstance()->init();
	//APFuturesMarketQuotations::getInstance()->subscribeInstrument(instrumentID);

	printf("0");
}