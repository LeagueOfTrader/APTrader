#include "APTimer.h"

HANDLE APTimer::ms_timerQueue = NULL;

VOID CALLBACK timerCallback(PVOID lpParameter, BOOLEAN TimerOrWaitFired) {
	APTimer* timer = (APTimer*)lpParameter;
	if (timer != NULL) {
		timer->trigger();
	}
}

APTimer::APTimer(TimerCallback callback, long delay, long interval, bool loop)
{
	m_valid = true;

	m_callback = callback;
	m_delay = delay;
	m_interval = interval;

	HANDLE timerQueue = getSharedTimerQueue();
	ULONG flag = WT_EXECUTEINTIMERTHREAD;
	if (!loop) {
		flag |= WT_EXECUTEONLYONCE;
	}
	CreateTimerQueueTimer(&m_timer, timerQueue, (WAITORTIMERCALLBACK)timerCallback, this, m_delay, m_interval, flag);
}

APTimer::~APTimer()
{
	m_valid = false;
	HANDLE timerQueue = getSharedTimerQueue();
	DeleteTimerQueueTimer(timerQueue, m_timer, NULL);
}

bool APTimer::isValid()
{
	return m_valid;
}

void APTimer::trigger()
{
	if (m_callback != NULL) {
		m_callback();
	}

	if (!m_loop) {
		m_valid = false;
	}
}

HANDLE APTimer::getSharedTimerQueue() {
	if (ms_timerQueue == NULL) {
		ms_timerQueue = CreateTimerQueue();
	}

	return ms_timerQueue;
}

void APTimer::destroyTimerQueue()
{
	if (ms_timerQueue != NULL) {
		DeleteTimerQueue(ms_timerQueue);
		ms_timerQueue = NULL;
	}
}

