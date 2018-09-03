#include "Date.h"

CDate::CDate() noexcept : day(0), month(0), year(0){ }

CDate::CDate(const CDate &obj) noexcept : day(obj.day), month(obj.month),\
									year(obj.year) { }

CDate::CDate(const unsigned day, \
				const unsigned month, \
				const unsigned year) noexcept {

	Set(day, month, year);
}

CDate &CDate::operator=(const CDate &obj) noexcept {

	day = obj.day;
	month = obj.month;
	year = obj.year;
	return *this;
}

CDate::~CDate() noexcept { }