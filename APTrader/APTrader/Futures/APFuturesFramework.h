#pragma once
#include "../APFramework.h"
#include "../APMacro.h"
#include "../APTypes.h"
#include "../Common/Singleton.h"
#include "../Common/InitializableObject.h"

#ifdef USE_CTP
const BYTE CTP_Flag_None = 0;
const BYTE CTP_Flag_Md = 1;
const BYTE CTP_Flag_Trade = 2;
const BYTE CTP_Flag_All = 3;
#endif

class APFuturesFramework : public APFramework, public Singleton<APFuturesFramework>
{
public:
	APFuturesFramework();
	~APFuturesFramework();

	//virtual void init();
	//virtual void update(float deltaTime);
	//virtual void exit();
	virtual void ready();
	virtual void start();

	virtual bool finished();

protected:
	void initLocalSystem();

#ifdef USE_CTP
public:
	void onCTPApiInited(BYTE flag);

protected:
	void initCTP();

private:
	BYTE m_initFlag;
#endif

};

