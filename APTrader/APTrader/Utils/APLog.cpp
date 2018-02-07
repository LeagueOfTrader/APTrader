#include "APLog.h"
#include <iostream>

#define LOG_BUF_SIZE 1024

#define OUTPUT_DBG_WND

#ifdef OUTPUT_DBG_WND
#include <windows.h>

std::wstring s2ws(std::string s)
{
	const char* Source = s.c_str();
	size_t size = s.size() + 1;
	wchar_t* Dest = new wchar_t[size];
	wmemset(Dest, 0, size);
	size_t len = 0;
	mbstowcs(Dest, Source, size);
	//mbstowcs_s(&len, Dest, size, Source, size);  
	std::wstring result = Dest;
	delete[] Dest;

	return result;
}
#endif //OUTPUT_DBG_WND


APLog::APLog()
{
}


APLog::~APLog()
{
}

void APLog::log(const char* charFormat, ...) {
	va_list args;
	va_start(args, charFormat);
	std::string content = getFormatStr(charFormat, args);
	va_end(args);
#ifdef _DEBUG
	std::cout << content << std::endl;
#else
	// output to file
#endif

#ifdef OUTPUT_DBG_WND
	OutputDebugString(s2ws(content + "\n").c_str());
	//OutputDebugString((content + "\n").c_str());
#endif
}

std::string APLog::getFormatStr(const char* charFormat, va_list args) {
	char buf[LOG_BUF_SIZE];
	memset(buf, 0, sizeof(buf));
	vsprintf_s(buf, charFormat, args);

	std::string result = buf;
	return result;
}