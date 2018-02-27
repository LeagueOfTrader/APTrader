#include "APFramework.h"
#include "Common/InitializableObject.h"
#include "Common/Ticker.h"
#include <windows.h>

APFramework::APFramework()
{
	m_finished = false;
	m_pause = false;
}


APFramework::~APFramework()
{
}

void APFramework::preInit()
{
	for (int i = 0; i < m_preInitList.size(); i++) {
		InitializableObject* obj = m_preInitList[i];
		if (obj != NULL) {
			obj->init();
		}
	}
}

void APFramework::postInit()
{
	for (int i = 0; i < m_postInitList.size(); i++) {
		InitializableObject* obj = m_postInitList[i];
		if (obj != NULL) {
			obj->init();
		}
	}
}

void APFramework::init()
{
	for (int i = 0; i < m_initList.size(); i++) {
		InitializableObject* obj = m_initList[i];
		if (obj != NULL) {
			obj->init();
		}
	}
}

void APFramework::update()
{
	if (m_pause) {
		return;
	}

	long curTick = GetTickCount();
	for (int i = 0; i < m_tickList.size(); i++) {
		Ticker* ticker = m_tickList[i];
		if (ticker != NULL) {
			ticker->tick(curTick);
		}
	}
}

void APFramework::exit()
{
	for (int i = m_postInitList.size() - 1; i >= 0; i--) {
		InitializableObject* obj = m_postInitList[i];
		if (obj == NULL) {
			continue;
		}
		obj->exit();
	}

	for (int i = m_initList.size() - 1; i >= 0; i--) {
		InitializableObject* obj = m_initList[i];
		if (obj == NULL) {
			continue;
		}
		obj->exit();
	}

	for (int i = m_preInitList.size() - 1; i >= 0; i--) {
		InitializableObject* obj = m_preInitList[i];
		if (obj == NULL) {
			continue;
		}
		obj->exit();
	}
}

bool APFramework::finished()
{
	return m_finished;
}

void APFramework::terminate()
{
	m_finished = true;
}

void APFramework::addInit(InitializableObject * initObj)
{
	m_initList.push_back(initObj);
}

void APFramework::addPreInit(InitializableObject * initObj)
{
	m_preInitList.push_back(initObj);
}

void APFramework::addPostInit(InitializableObject * initObj)
{
	m_postInitList.push_back(initObj);
}

void APFramework::addTicker(Ticker * ticker)
{
	m_tickList.push_back(ticker);
}

bool APFramework::inited()
{
	for (int i = 0; i < m_initList.size(); i++) {
		InitializableObject* obj = m_initList[i];
		if (obj == NULL) {
			return false;
		}

		if (!obj->inited()) {
			return false;
		}
	}
	return true;
}

bool APFramework::exited()
{
	for (int i = m_postInitList.size() - 1; i >= 0; i--) {
		InitializableObject* obj = m_postInitList[i];
		if (obj == NULL) {
			continue;
		}

		if (!obj->exited()) {
			return false;
		}
	}

	for (int i = m_initList.size() - 1; i >= 0; i--) {
		InitializableObject* obj = m_initList[i];
		if (obj == NULL) {
			continue;
		}

		if (!obj->exited()) {
			return false;
		}
	}

	for (int i = m_preInitList.size() - 1; i >= 0; i--) {
		InitializableObject* obj = m_preInitList[i];
		if (obj == NULL) {
			continue;
		}

		if (!obj->exited()) {
			return false;
		}
	}

	return true;
}

void APFramework::setPause(bool pause)
{
	m_pause = pause;
}
