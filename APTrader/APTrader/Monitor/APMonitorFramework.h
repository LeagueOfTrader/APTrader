#pragma once
#include "../Common/Singleton.h"
#include "../Common/InitializableObject.h"
#include "../APFramework.h"

class APMonitorFramework : public APFramework, public InitializableObject, public Singleton<APMonitorFramework>
{
public:
	APMonitorFramework();
	~APMonitorFramework();

	virtual void init();
	virtual void update(float deltaTime);
	virtual void exit();

	void onRemoteInited();
	void onLocalInited();

protected:
	void initRemoteSystem();
	
	void initLocalSystem();

};

