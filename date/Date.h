#pragma once
#include <assert.h>
#include <basic/chars.h>
#include "dll_definitions.h"

class DATELIB_DLL_EXPORT CDate {
	unsigned day;
	unsigned month;
	unsigned year;

public:
	enum Formats {
		SQL_FORMAT = 1,\
		GERMAN_FORMAT = 2
	};
	enum Lengthes {
		SQL_FORMAT_LEN = 10, \
		GERMAN_FORMAT_LEN = 10
	};
	CDate() noexcept;
	CDate(const CDate &obj) noexcept;
	CDate(CDate &&obj) = default;

	CDate(const unsigned day, \
			const unsigned month, \
			const unsigned year) noexcept;

	template <typename Tchar> \
		CDate(const Tchar *date_str, const int format) noexcept;
	CDate &operator=(const CDate &obj) noexcept;
	CDate &operator=(CDate &&obj) = default;

	inline bool operator==(const CDate &obj) const noexcept;
	inline bool operator!=(const CDate &obj) const noexcept;
	inline bool operator>(const CDate &obj) const noexcept;
	inline bool operator<(const CDate &obj) const noexcept;
	inline bool operator>=(const CDate &obj) const noexcept;
	inline bool operator<=(const CDate &obj) const noexcept;

	inline bool isNull() const noexcept;
	inline void clear() noexcept { day = 0; month = 0; year = 0; }

	inline unsigned getDay() const noexcept { return day; }
	inline unsigned getMonth() const noexcept { return month; }
	inline unsigned getYear() const noexcept { return year; }
	static CDate now();

	inline void set(const unsigned day, \
					const unsigned month, \
					const unsigned year) noexcept;

	template <typename Tchar> \
		inline bool setDateSQL(const Tchar *date_str) noexcept;
	template <typename Tchar> \
		inline bool setDateGerman(const Tchar *date_str) noexcept;

	template <typename Tchar> \
		inline void toStringGerman(Tchar *buffer) const noexcept;
	template <typename Tchar> \
		inline void toStringSQL(Tchar *buffer) const noexcept;

	virtual ~CDate() noexcept;
};

//****************************************************************************

template <typename Tchar> \
CDate::CDate(const Tchar *date_str, const int format) noexcept : \
				day(0), month(0), year(0) {

	switch (format) {
	case SQL_FORMAT:
		setDateSQL(date_str);

	case GERMAN_FORMAT:
		setDateGerman(date_str);
	}
}

bool CDate::operator==(const CDate &obj) const noexcept {

	return day == obj.day && month == obj.month && year == obj.year;
}

bool CDate::operator!=(const CDate &obj) const noexcept {

	return !operator==(obj);
}

bool CDate::operator>(const CDate &obj) const noexcept {

	return year > obj.year || \
			(year == obj.year && month > obj.month) || \
			(year == obj.year && month == obj.month && day > obj.day);
}

bool CDate::operator<(const CDate &obj) const noexcept {

	return year < obj.year || \
			(year == obj.year && month < obj.month) || \
			(year == obj.year && month == obj.month && day < obj.day);
}

bool CDate::operator>=(const CDate &obj) const noexcept {

	return !operator<(obj);
}

bool CDate::operator<=(const CDate &obj) const noexcept {

	return !operator>(obj);
}

bool CDate::isNull() const noexcept {

	return day == 0 && month == 0 && year == 0;
}

void CDate::set(const unsigned day, \
				const unsigned month, \
				const unsigned year) noexcept {

	assert(day <= 31 && month <= 12);
	this->day = day;
	this->month = month;
	this->year = year;
}

template <typename Tchar> \
bool CDate::setDateSQL(const Tchar *date_str) noexcept {
	bool key = false;
	unsigned Day(day), Month(month), Year(year);
	const Tchar _0 = getZeroChar(Tchar());
	const Tchar _1 = getOneChar(Tchar());
	const Tchar _9 = getNineChar(Tchar());
	const Tchar _minus = getMinusChar(Tchar());

	key = date_str[0] >= _0 && date_str[0] <= _9;
	key = key && (date_str[1] >= _0 && date_str[1] <= _9);
	key = key && (date_str[2] >= _0 && date_str[2] <= _9);
	key = key && (date_str[3] >= _0 && date_str[3] <= _9);
	Year = 1000 * (date_str[0] - _0) + 100 * (date_str[1] - _0);
	Year += 10 * (date_str[2] - _0) + date_str[3] - _0;
	key = key && (date_str[4] == _minus);

	key = key && (date_str[5] >= _0 && date_str[5] <= _1);
	key = key && (date_str[6] >= _0 && date_str[6] <= _9);
	Month = 10 * (date_str[5] - _0) + date_str[6] - _0;
	key = key && (date_str[7] == _minus);

	key = key && (date_str[8] >= _0 && date_str[8] <= _9);
	key = key && (date_str[9] >= _0 && date_str[9] <= _9);
	Day = 10 * (date_str[8] - _0) + date_str[9] - _0;

	if (!key || Day > 31 || Month > 12) return false;
	day = Day; month = Month; year = Year;
	return true;
}

template <typename Tchar> \
bool CDate::setDateGerman(const Tchar *date_str) noexcept {
	bool key = false;
	unsigned Day(day), Month(month), Year(year);
	const Tchar _0 = getZeroChar(Tchar());
	const Tchar _1 = getOneChar(Tchar());
	const Tchar _9 = getNineChar(Tchar());
	const Tchar _dot = getDotChar(Tchar());

	key = date_str[0] >= _0 && date_str[0] <= _9;
	key = key && (date_str[1] >= _0 && date_str[1] <= _9);
	Day = 10 * (date_str[0] - _0) + date_str[1] - _0;
	key = key && (date_str[2] == _dot);

	key = key && (date_str[3] >= _0 && date_str[3] <= _1);
	key = key && (date_str[4] >= _0 && date_str[4] <= _9);
	Month = 10 * (date_str[3] - _0) + date_str[4] - _0;
	key = key && (date_str[5] == _dot);

	key = key && (date_str[6] >= _0 && date_str[6] <= _9);
	key = key && (date_str[7] >= _0 && date_str[7] <= _9);
	key = key && (date_str[8] >= _0 && date_str[8] <= _9);
	key = key && (date_str[9] >= _0 && date_str[9] <= _9);
	Year = date_str[6] - _0;
	Year = 1000 * (date_str[6] - _0) + 100 * (date_str[7] - _0);
	Year += 10 * (date_str[8] - _0) + date_str[9] - _0;

	if (!key || Day > 31 || Month > 12) return false;
	day = Day; month = Month; year = Year;
	return true;
}

template <typename Tchar> \
void CDate::toStringGerman(Tchar *buffer) const noexcept {
	const Tchar _0 = getZeroChar(Tchar());
	const Tchar _dot = getDotChar(Tchar());

	assert(buffer);
	buffer[0] = _0 + day / 10;
	buffer[1] = _0 + day % 10;
	buffer[2] = _dot;

	buffer[3] = _0 + month / 10;
	buffer[4] = _0 + month % 10;
	buffer[5] = _dot;

	unsigned Year = year;
	buffer[9] = _0 + Year % 10;
	Year /= 10;
	buffer[8] = _0 + Year % 10;
	Year /= 10;
	buffer[7] = _0 + Year % 10;
	Year /= 10;
	buffer[6] = _0 + Year % 10;

	buffer[10] = 0;
}

template <typename Tchar> \
void CDate::toStringSQL(Tchar *buffer) const noexcept {
	unsigned Year = year;
	const Tchar _0 = getZeroChar(Tchar());
	const Tchar _minus = getMinusChar(Tchar());

	assert(buffer);
	buffer[3] = _0 + Year % 10;
	Year /= 10;
	buffer[2] = _0 + Year % 10;
	Year /= 10;
	buffer[1] = _0 + Year % 10;
	Year /= 10;
	buffer[0] = _0 + Year % 10;
	buffer[4] = _minus;

	buffer[5] = _0 + month / 10;
	buffer[6] = _0 + month % 10;
	buffer[7] = _minus;

	buffer[8] = _0 + day / 10;
	buffer[9] = _0 + day % 10;

	buffer[10] = 0;
}
