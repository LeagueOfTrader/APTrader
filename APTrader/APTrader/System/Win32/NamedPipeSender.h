#pragma once
#include <string>
#include <Windows.h>
#include "../../Common/InitializableObject.h"

class NamedPipeSender : public InitializableObject
{
public:
	NamedPipeSender(std::string pipeName);
	~NamedPipeSender();

	virtual void init();
	bool send(void* buf, long bufSize, unsigned long len);

private:
	HANDLE m_hPipe;
	std::string m_pipeName;
};