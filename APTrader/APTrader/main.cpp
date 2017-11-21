#include "Strategy/APStandardGridStrategy.h"
#include "APFramework.h"
#include "APStrategyManager.h"
#include "APStrategyFactory.h"
#include <windows.h>
#include "APGlobalConfig.h"
#include "Utils/APJsonReader.h"
#include "Simulation/APSimFramework.h"

void main() {
	//APGlobalConfig::getInstance()->init();

	//APStrategyFactory::init();

	//APStandardGridStrategy* gs = (APStandardGridStrategy*)APStrategyManager::getInstance()->createStrategy("Data/grid0.stg");
	//gs->printGrids();

	//APJsonReader jr;
	//jr.initWithFile("Data/test.cfg");
	//int n = jr.getArraySize("SubStrategies");
	//std::string ae = jr.getArrayElement("SubStrategies", 0);

	//gs->update();
	APSimFramework* framework = new APSimFramework();
	if (framework != NULL)
	{
		framework->init();

		//APStandardGridStrategy* gs = (APStandardGridStrategy*)APStrategyManager::getInstance()->createStrategy("Data/grid0.stg");
		//gs->printGrids();
		
		while (!framework->finished()) {
			framework->update();
			Sleep(300);
		}
	}
	framework->exit();
	delete framework;
}