#pragma once
#include "../APFramework.h"
#include "../APMacro.h"
#include "../APTypes.h"
#include "../Common/Singleton.h"
#include "../Common/InitializableObject.h"
#ifdef USE_CTP

class APTestFramework : public APFramework, public InitializableObject, public Singleton<APTestFramework> 
{
public:
	APTestFramework();
	~APTestFramework();

	virtual void init();
	virtual void update(float deltaTime);
	virtual void exit();

	virtual bool finished();

protected:
	void initLocalSystem();

public:
	void onCTPApiInited();

protected:
	void initCTP();

private:
	int m_ctpCount;
#endif
};