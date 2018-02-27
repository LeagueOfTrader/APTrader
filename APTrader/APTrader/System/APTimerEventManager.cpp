#include "APTimerEventManager.h"

void APTimerEventManager::registerEvent(TimerCallback callback, long delay, long interval, bool loop)
{
	APTimer* timer = new APTimer(callback, delay, interval, loop);
	m_timers.push_back(timer);
}

void APTimerEventManager::destroy()
{
	std::list<APTimer*>::iterator it;
	for (it = m_timers.begin(); it != m_timers.end(); it++) {
		APTimer* timer = *it;
		if (timer != NULL) {
			delete timer;
		}
	}

	m_timers.clear();

	APTimer::destroyTimerQueue();
}