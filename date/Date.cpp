#include "Date.h"
#include <chrono>

CDate::CDate() noexcept : day(0), month(0), year(0) { }

CDate::CDate(const CDate &obj) noexcept : day(obj.day), month(obj.month),\
									year(obj.year) { }

CDate::CDate(const unsigned day, \
				const unsigned month, \
				const unsigned year) noexcept {

	set(day, month, year);
}

CDate &CDate::operator=(const CDate &obj) noexcept {

	day = obj.day;
	month = obj.month;
	year = obj.year;
	return *this;
}

CDate CDate::now() {

	auto Now = std::chrono::system_clock::now();
	time_t time = std::chrono::system_clock::to_time_t(Now);

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4996)
#endif
	auto tm = localtime(&time);
#ifdef _MSC_VER
#pragma warning(pop)
#endif
	assert(tm);

	return CDate(tm->tm_mday, tm->tm_mon + 1, tm->tm_year + 1900);
}

CDate::~CDate() noexcept { }