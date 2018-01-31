#pragma once
#include <thread>
#include "../Common/InitializableObject.h"
#include "../Common/Singleton.h"
#include "../Common/Ticker.h"
#include <mutex>
#include <deque>
#include <string>

class APInputSystem : public InitializableObject, public Ticker, public Singleton<APInputSystem>
{
public:
	APInputSystem();
	~APInputSystem();

	virtual void init();
	virtual void exit();
	virtual void update();

	void listenInput();

private:
	void pushCommand(std::string cmd);

private:
	std::deque<std::string> m_commandBuffer;
	std::thread m_inputThread;
	std::mutex m_cmdBufferMutex;
};

