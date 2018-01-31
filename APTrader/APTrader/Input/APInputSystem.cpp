#include "APInputSystem.h"

void inputThread() {
	APInputSystem::getInstance()->listenInput();
}

APInputSystem::APInputSystem()
{
}


APInputSystem::~APInputSystem()
{
}

void APInputSystem::init() 
{
	m_inputThread = std::thread(inputThread);
	setInited();
}

void APInputSystem::exit() 
{
	m_inputThread.detach();
	m_exited = true;
}

void APInputSystem::update()
{
	m_cmdBufferMutex.lock();
	while (m_commandBuffer.size() > 0) {
		std::string& cmd = m_commandBuffer.front();
		m_commandBuffer.pop_front();
	}
	m_cmdBufferMutex.unlock();
}

void APInputSystem::listenInput()
{
	char cmd[128];
	while (scanf("%s", &cmd) != EOF) {
		pushCommand(cmd);
	}
}

void APInputSystem::pushCommand(std::string cmd)
{
	m_cmdBufferMutex.lock();
	m_commandBuffer.push_back(cmd);
	m_cmdBufferMutex.unlock();
}
