#include "APInputSystem.h"
#include "APInputCommandParser.h"
#include <functional>

//void inputThread() {
//	APInputSystem::getInstance()->listenInput();
//}

void onGetInput(std::string cmd) {
	APInputSystem::getInstance()->pushCommand(cmd);
}

APInputSystem::APInputSystem()
{
	m_pipeListener = NULL;
}


APInputSystem::~APInputSystem()
{
}

void APInputSystem::init() 
{
	m_pipeListener = new NamedPipeListener("APTrader_KeyboardInput");
	bool ret = m_pipeListener->inited();
	if (ret) {
		m_pipeListener->registerPipeCallback(onGetInput);
		m_pipeListener->start();
		setInited();
	}
	
}

void APInputSystem::exit() 
{
	//m_inputThread.detach();
	if (m_pipeListener != NULL) {
		delete m_pipeListener;
		m_pipeListener = NULL;
	}
	m_exited = true;
}

void APInputSystem::update()
{
	m_cmdBufferMutex.lock();
	while (m_commandBuffer.size() > 0) {
		std::string& cmd = m_commandBuffer.front();
		APInputCommandParser::parseCommand(cmd);
		m_commandBuffer.pop_front();
	}
	m_cmdBufferMutex.unlock();
}

//void APInputSystem::listenInput()
//{
//	char cmd[128];
//	while (scanf("%s", &cmd) != EOF) {
//		pushCommand(cmd);
//	}
//}

void APInputSystem::pushCommand(std::string cmd)
{
	m_cmdBufferMutex.lock();
	m_commandBuffer.push_back(cmd);
	m_cmdBufferMutex.unlock();
}
