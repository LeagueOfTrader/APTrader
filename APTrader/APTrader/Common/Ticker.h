#pragma once

#include <windows.h>

class Ticker 
{
public:
	Ticker(long interval = 10) {
		m_lastTick = 0;

		m_interval = interval;
	}

	void setInterval(long interval) {
		m_interval = interval;
	}

	void tick() {
		long curTick = GetTickCount();
		tick(curTick);
	}

	void tick(long curTick) {
		if (curTick - m_lastTick >= m_interval) {
			update();
			m_lastTick = curTick;
		}
	}

	virtual void update() = 0;

private:
	long m_lastTick;
	long m_interval;
};