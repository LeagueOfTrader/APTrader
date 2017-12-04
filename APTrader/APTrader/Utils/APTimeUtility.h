#pragma once
#include <string>
#include "../APTypes.h"

class APTimeUtility
{
public:
	static std::string getCurYearMonthInFuturesFormat();

	static std::string addMonthToYearMonth(std::string ym, int m);
	static int compareFuturesYearMonth(std::string ym1, std::string ym2); // -1: ym1 early, 0: equal, 1: ym1 late

	static int getYear(std::string ym);
	static int getMonth(std::string ym);
	static std::string generateYearMonth(int year, int month);

	static UINT getMonthsToFuturesDeliveryDate(std::string ym);
	static UINT getDaysToFuturesDeliveryDate(std::string ym);
	static UINT getWorkDaysToFuturesDeliveryDate(std::string ym);

	static UINT getTimestamp();
};

