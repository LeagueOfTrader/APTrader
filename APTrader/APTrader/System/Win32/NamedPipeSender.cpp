#include "NamedPipeSender.h"

NamedPipeSender::NamedPipeSender(std::string pipeName)
{
	m_pipeName = "\\\\.\\pipe\\" + pipeName;
	m_hPipe = INVALID_HANDLE_VALUE;
}

NamedPipeSender::~NamedPipeSender()
{
}

void NamedPipeSender::init()
{
	bool ret = WaitNamedPipeA(m_pipeName.c_str(), NMPWAIT_WAIT_FOREVER);
	if (ret) {
		m_hPipe = CreateFileA(m_pipeName.c_str(), GENERIC_WRITE, 0,
			NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	}

	if (m_hPipe != INVALID_HANDLE_VALUE) {
		setInited();
	}
}

bool NamedPipeSender::send(void * buf, long bufSize, unsigned long & len)
{
	bool ret = false;
	if (m_hPipe != INVALID_HANDLE_VALUE) {
		ret = WriteFile(m_hPipe, buf, bufSize, &len, NULL);
	}

	return ret;
}
