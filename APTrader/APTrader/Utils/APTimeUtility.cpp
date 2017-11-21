#include "APTimeUtility.h"
#include <windows.h>

UINT MONTH_DAYS_APPROXIMATION = 30;
UINT DAYS_IN_MONTH[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

std::string APTimeUtility::getCurYearMonthInFuturesFormat()
{
	SYSTEMTIME sys;
	GetLocalTime(&sys);
	int year = sys.wYear;
	int month = sys.wMonth;
	int yearPrefix = year / 100 * 100;
	year = year - yearPrefix;
	
	std::string yearMonth = generateYearMonth(year, month);
	return yearMonth;
}

std::string APTimeUtility::addMonthToYearMonth(std::string ym, int m)
{
	int year = getYear(ym);
	int month = getMonth(ym);
	month += m;
	
	if (month > 12) {
		int yearAccum = month / 12;
		month -= 12 * yearAccum;
		year += yearAccum;
	}

	return generateYearMonth(year, month);
}

int APTimeUtility::compareFuturesYearMonth(std::string ym1, std::string ym2)
{
	//if (ym1.compare(ym2) == 0) {
	//	return 0;
	//}

	int y1 = getYear(ym1);
	int y2 = getYear(ym2);
	if (y1 < y2) {
		return -1;
	}
	else if (y1 > y2) {
		return 1;
	}

	int m1 = getMonth(ym1);
	int m2 = getMonth(ym2);
	if (m1 < m2) {
		return -1;
	}
	else if (m1 > m2) {
		return 1;
	}

	return 0;
}

int APTimeUtility::getYear(std::string ym)
{
	std::string yr = ym.substr(0, 2);
	return std::stoi(yr);
}

int APTimeUtility::getMonth(std::string ym)
{
	std::string mt = ym.substr(2);
	return std::stoi(mt);
}

std::string APTimeUtility::generateYearMonth(int year, int month)
{
	char* ym = new char[8];
	sprintf(ym, "%2d%2d", year, month);
	std::string yearMonth = ym;
	return yearMonth;
}

UINT APTimeUtility::getMonthsToFuturesDeliveryDate(std::string ym)
{
	int targetYear = APTimeUtility::getYear(ym);
	int targetMonth = APTimeUtility::getMonth(ym);

	SYSTEMTIME sys;
	GetLocalTime(&sys);
	int year = sys.wYear;
	int month = sys.wMonth;
	int yearPrefix = year / 100 * 100;
	year = year - yearPrefix;

	UINT months = (year - targetYear) * 12 + (targetMonth - month);
	return months;
}

UINT APTimeUtility::getDaysToFuturesDeliveryDate(std::string ym)
{
	UINT months = getMonthsToFuturesDeliveryDate(ym);
	if (months == 0) {
		return 0;
	}

	UINT days = MONTH_DAYS_APPROXIMATION * (months - 1);
	SYSTEMTIME sys;
	GetLocalTime(&sys);
	int curMonth = sys.wMonth;
	UINT daysInMonth = DAYS_IN_MONTH[curMonth - 1];
	int curDay = sys.wDay;
	days += daysInMonth - curDay;

	return days;
}

UINT APTimeUtility::getWorkDaysToFuturesDeliveryDate(std::string ym)
{
	return 0;
}
