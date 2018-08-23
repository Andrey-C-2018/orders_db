#include "Date.h"

CDate::CDate() noexcept : day(0), month(0), year(0){ }

CDate::CDate(const CDate &obj) noexcept : day(obj.day), month(obj.month),\
									year(obj.year) { }

CDate::CDate(const unsigned day, \
				const unsigned month, \
				const unsigned year) noexcept {

Set(day, month, year);
}

template <typename Tchar> \
CDate::CDate(const Tchar *date_str, const int format) noexcept : \
				day(0), month(0), year(0) {

switch (format) {
	case SQL_FORMAT:
		SetDateSQL(date_str);

	case GERMAN_FORMAT:
		SetDateGerman(date_str);
	}
}

CDate &CDate::operator=(const CDate &obj) noexcept {

day = obj.day;
month = obj.month;
year = obj.year;
return *this;
}

CDate::~CDate() noexcept { }