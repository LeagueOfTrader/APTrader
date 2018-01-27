#pragma once
#include <string>
#include <vector>
#include "Common/InitializableObject.h"

class Ticker;
class InitializableObject;

class APFramework : public InitializableObject
{
public:
	APFramework();
	~APFramework();

	virtual void preInit();
	virtual void postInit();
	
	virtual void init();
	virtual void update();
	virtual void exit();

	virtual bool finished();
	void terminate();

	void addInit(InitializableObject* initObj);
	void addPreInit(InitializableObject* initObj);
	void addPostInit(InitializableObject* initObj);

	void addTicker(Ticker* ticker);

	bool inited();
	bool exited();

protected:
	bool m_finished;
	std::vector<InitializableObject*> m_preInitList;
	std::vector<InitializableObject*> m_initList;
	std::vector<InitializableObject*> m_postInitList;

	std::vector<Ticker*> m_tickList;
};

