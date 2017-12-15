#pragma once
#include <string>

class APFramework
{
public:
	APFramework();
	~APFramework();

	virtual void init() = 0;
	virtual void update(float deltaTime = 0.0f) = 0;
	virtual void exit() = 0;

	virtual bool finished();

protected:
	bool m_finished;
};

