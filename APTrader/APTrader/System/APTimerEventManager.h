#pragma once
#include "../Common/Singleton.h"
#include "Win32/APTimer.h"

#include <list>

class APTimerEventManager : public Singleton<APTimerEventManager>
{
public:
	void destroy();
	void registerEvent(TimerCallback callback, long delay, long interval, bool loop = true);

protected:
	std::list<APTimer*> m_timers;
};

