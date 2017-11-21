#pragma once
#include "../Common/Singleton.h"
#include <string>
#include <stdarg.h>

class APLog : public Singleton<APLog>
{
public:
	APLog();
	~APLog();

	void log(const char* charFormat, ...);

private:
	inline std::string getFormatStr(const char* charFormat, va_list args);
};

#define APLogger APLog::getInstance()

