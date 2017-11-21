#pragma once
#include <string>

class APFramework
{
public:
	APFramework();
	~APFramework();

	virtual void init();
	virtual void update();
	virtual void exit();

	virtual bool finished();

protected:
	bool m_finished;
};

