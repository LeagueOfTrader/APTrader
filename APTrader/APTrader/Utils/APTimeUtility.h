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

	static time_t getTimestamp();

	static std::string getDate();
	static std::string getDateInDateTime(std::string dateTime);
	static std::string getDateTime();
	static std::string getLastTransactionDay();
	static std::string getNextTransactionDay();
	static int compareDateTime(std::string dt0, std::string dt1);
	static int compareDate(std::string dt0, std::string dt1);

	static std::string makeUpDateTime(std::string date, UINT hour, UINT minute = 0, UINT second = 0);
	static std::string makeUpDateTime(UINT year, UINT month, UINT day, UINT hour, UINT minute = 0, UINT second = 0);
	static UINT calcDeltaSeconds(std::string dt0, std::string dt1);
	
	static std::string calcDateByDeltaDays(std::string srcDate, int deltaDays);
	static UINT getYearInDateTime(std::string dateTime);
	static UINT getMonthInDateTime(std::string dateTime);
	static UINT getDayInDateTime(std::string dateTime);
	static UINT getHourInDateTime(std::string dateTime);
	static UINT getMinuteInDateTime(std::string dateTime);
	static UINT getSecondInDateTime(std::string dateTime);
};

