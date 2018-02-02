#pragma once

#include <string>
#include <Windows.h>
#include "../../Common/InitializableObject.h"
#include <mutex>
#include <thread>
#include "../../Common/Singleton.h"

typedef void(*PipeCallback)(std::string msg);

class NamedPipeListener : public InitializableObject
{
public:
	NamedPipeListener(std::string pipeName);
	~NamedPipeListener();

	virtual void init();
	void start();

	void listenPipe();

	void setRun(bool run);
	void registerPipeCallback(PipeCallback callback);

protected:
	std::string m_pipeName;
	std::mutex m_mutex;
	std::thread m_listenThread;

	HANDLE m_hPipe;
	bool m_run;
	LPCRITICAL_SECTION m_criticalSection;
	HANDLE m_semaphore;
	PipeCallback m_pipeCallback;
};