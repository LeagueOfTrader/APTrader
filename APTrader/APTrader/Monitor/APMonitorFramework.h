#pragma once
#include "../Common/Singleton.h"
#include "../APFramework.h"

class APMonitorFramework : public APFramework, public Singleton<APMonitorFramework>
{
public:
	APMonitorFramework();
	~APMonitorFramework();

	virtual void init();
	virtual void update(float deltaTime);
	virtual void exit();

	bool inited();
	void onRemoteInited();
	void onLocalInited();

protected:
	void initRemoteSystem();
	
	void initLocalSystem();
	

private:
	bool m_inited;
};

