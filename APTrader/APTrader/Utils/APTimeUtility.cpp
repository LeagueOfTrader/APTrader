#include "APTimeUtility.h"
#include <windows.h>
#include <time.h>

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
	// todo: search calender...
	return 0;
}

UINT APTimeUtility::getTimestamp()
{
	time_t t = time(0);
	localtime(&t);
	return t;
}

std::string APTimeUtility::getDateTime()
{
	SYSTEMTIME sys;
	GetLocalTime(&sys);
	char strDateTime[64];
	sprintf(strDateTime, "%4d%2d%2d%2d%2d%2d", sys.wYear, sys.wMonth, sys.wDay, sys.wHour, sys.wMinute, sys.wSecond);
	std::string dateTime = strDateTime;
	return dateTime;
}

std::string APTimeUtility::getLastFutureTransactionDay()
{
	SYSTEMTIME sys;
	GetLocalTime(&sys);
	if (sys.wDayOfWeek == 0 || sys.wDayOfWeek == 6) {
		//
	}
	char strDateTime[64];
	sprintf(strDateTime, "%4d%2d%2d%2d%2d%2d", sys.wYear, sys.wMonth, sys.wDay, sys.wHour, sys.wMinute, sys.wSecond);
	std::string dateTime = strDateTime;
	return dateTime;
}

int APTimeUtility::compareDateTime(std::string dt0, std::string dt1)
{
	UINT y0 = getYearInDateTime(dt0);
	UINT y1 = getYearInDateTime(dt1);
	if (y0 > y1) {
		return 1;
	}
	else if (y0 < y1) {
		return -1;
	}

	UINT m0 = getMonthInDateTime(dt0);
	UINT m1 = getMonthInDateTime(dt1);
	if (m0 > m1) {
		return 1;
	}
	else if (m0 < m1) {
		return -1;
	}

	UINT d0 = getDayInDateTime(dt0);
	UINT d1 = getDayInDateTime(dt1);
	if (d0 > d1) {
		return 1;
	}
	else if (d0 < d1) {
		return -1;
	}

	UINT h0 = getHourInDateTime(dt0);
	UINT h1 = getHourInDateTime(dt1);
	if (h0 > h1) {
		return 1;
	}
	else if (h0 < h1) {
		return -1;
	}

	UINT min0 = getMinuteInDateTime(dt0);
	UINT min1 = getMinuteInDateTime(dt1);
	if (min0 > min1) {
		return 1;
	}
	else if (min0 < min1) {
		return -1;
	}

	UINT sec0 = getSecondInDateTime(dt0);
	UINT sec1 = getSecondInDateTime(dt1);
	if (sec0 > sec1) {
		return 1;
	}
	else if (sec0 < sec1) {
		return -1;
	}

	return 0;
}

std::string APTimeUtility::calcDateByDeltaDays(std::string srcDate, int deltaDays) // delta < 1 month
{
	SYSTEMTIME sys;
	GetLocalTime(&sys);
	int year = sys.wYear;
	int month = sys.wMonth;
	int day = sys.wDay;
	day += deltaDays;
	if (day <= 0) {
		month--;
		if (month <= 0) {
			month = 12;
			year--;
		}
		day += DAYS_IN_MONTH[month];
	}
	else if (day > DAYS_IN_MONTH[month]) {
		day -= DAYS_IN_MONTH[month++];
		if (month > 12) {
			month = 1;
		}
	}

	char strDateTime[64];
	sprintf(strDateTime, "%4d%2d%2d", sys.wYear, sys.wMonth, sys.wDay);
	std::string ymd = strDateTime;

	return ymd;
}

UINT APTimeUtility::getYearInDateTime(std::string dateTime)
{
	std::string str = dateTime.substr(0, 4);
	return atoi(str.c_str());
}

UINT APTimeUtility::getMonthInDateTime(std::string dateTime)
{
	std::string str = dateTime.substr(4, 2);
	return atoi(str.c_str());
}

UINT APTimeUtility::getDayInDateTime(std::string dateTime)
{
	std::string str = dateTime.substr(6, 2);
	return atoi(str.c_str());
}

UINT APTimeUtility::getHourInDateTime(std::string dateTime)
{
	std::string str = dateTime.substr(8, 2);
	return atoi(str.c_str());
}

UINT APTimeUtility::getMinuteInDateTime(std::string dateTime)
{
	std::string str = dateTime.substr(10, 2);
	return atoi(str.c_str());
}

UINT APTimeUtility::getSecondInDateTime(std::string dateTime)
{
	std::string str = dateTime.substr(12, 2);
	return atoi(str.c_str());
}
