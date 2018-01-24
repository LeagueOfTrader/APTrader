#pragma once
#include "../APFramework.h"
#include "../APMacro.h"
#include "../APTypes.h"
#include "../Common/Singleton.h"
#ifdef USE_CTP

class APTestFramework : public APFramework, public Singleton<APTestFramework> 
{
public:
	APTestFramework();
	~APTestFramework();

	virtual void init();
	virtual void update(float deltaTime);
	virtual void exit();

	virtual bool finished();

	bool inited();

protected:
	void initLocalSystem();

public:
	void onCTPApiInited();

protected:
	void initCTP();

private:
	int m_ctpCount;
	bool m_inited;
#endif
};