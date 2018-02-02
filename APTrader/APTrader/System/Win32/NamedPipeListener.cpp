#include "NamedPipeListener.h"

const std::string pipeNamePrefix = "\\\\.\\pipe\\";
const long pipeInterval = 500;
const int BUFFER_MAX_LEN = 1024;

NamedPipeListener::NamedPipeListener(std::string pipeName)
{
	m_pipeName = pipeName;
}

NamedPipeListener::~NamedPipeListener()
{
	if (m_inited) {
		m_run = false;
		m_listenThread.detach();
	}

	if (m_criticalSection != NULL) {
		DeleteCriticalSection(m_criticalSection);
		m_criticalSection = NULL;
	}	
}

void NamedPipeListener::init()
{
	std::string name = pipeNamePrefix + m_pipeName;
	m_hPipe = CreateNamedPipeA(name.c_str(), PIPE_ACCESS_DUPLEX,
		PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT,
		PIPE_UNLIMITED_INSTANCES, 0, 0, NMPWAIT_WAIT_FOREVER, 0);
	if (m_hPipe == INVALID_HANDLE_VALUE) {
		return;
	}

	if (ConnectNamedPipe(m_hPipe, NULL) != NULL) {
		setInited();
		m_run = true;
	}

	m_criticalSection = (LPCRITICAL_SECTION)malloc(sizeof(LPCRITICAL_SECTION));
	m_semaphore = CreateSemaphore(NULL, 1, 1, TEXT("semaphore"));
}

void NamedPipeListener::start()
{
	if (!inited()) {
		return;
	}

	m_listenThread = std::thread(std::bind(&NamedPipeListener::listenPipe, this));
}

void NamedPipeListener::listenPipe()
{	
	char buf[BUFFER_MAX_LEN];
	DWORD dwLen;

	while (m_run)
	{
		WaitForSingleObject(m_semaphore, INFINITE);
		EnterCriticalSection(m_criticalSection);
		ReadFile(m_hPipe, buf, BUFFER_MAX_LEN, &dwLen, NULL);

		//int bufSize;
		//for (bufSize = 0; bufSize < (int)dwLen; bufSize++) {
		//	//putchar(buf[bufSize]);
		//}

		if (dwLen < BUFFER_MAX_LEN - 1) {
			buf[dwLen] = '\0';
		}
		//std::string strVal = buf;
		if (m_pipeCallback != NULL) {
			m_pipeCallback(buf);
		}

		LeaveCriticalSection(m_criticalSection);
		Sleep(pipeInterval);
		ReleaseSemaphore(m_semaphore, 1, NULL);
	}
}

void NamedPipeListener::setRun(bool run)
{
	m_run = run;
}

void NamedPipeListener::registerPipeCallback(PipeCallback callback)
{
	m_pipeCallback = callback;
}