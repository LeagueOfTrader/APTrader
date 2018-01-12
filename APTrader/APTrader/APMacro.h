#pragma once

#ifndef USE_CTP
#define USE_CTP
#endif // !USE_CTP

//#ifndef SIM
//#define SIM
//#endif

#ifdef TRADER_API_EXPORT
#define TRADER_API __declspec(dllexport)
#endif