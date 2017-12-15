#pragma once
#include "../APFramework.h"
class APFuturesFramework :
	public APFramework
{
public:
	APFuturesFramework();
	~APFuturesFramework();

	virtual void init();
	virtual void update(float deltaTime);
	virtual void exit();

	virtual bool finished();

};

