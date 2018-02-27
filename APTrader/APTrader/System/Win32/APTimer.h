#pragma once

#include <windows.h>

typedef void(*TimerCallback)();

class APTimer
{
public:
	APTimer(TimerCallback callback, long delay, long interval, bool loop = true);
	~APTimer();

	bool isValid();
	void trigger();

	static HANDLE getSharedTimerQueue();

	static void destroyTimerQueue();

protected:
	static HANDLE ms_timerQueue;

private:
	HANDLE m_timer;
	TimerCallback m_callback;
	long m_delay;
	long m_interval;
	bool m_loop;

	bool m_valid;
};

