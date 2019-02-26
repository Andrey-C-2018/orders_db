#pragma once
#include <mysql.h>
#include <basic/XString.h>
#include <basic/TextConv.h>
#include <date/Date.h>

class CMySQLVariant final {
	enum{
		DEF_BUFFER_SIZE = 30, \
		MAX_STR_SIZE = 255
	};

	enum_field_types value_type;
	union {
		int value_int;
		unsigned value_unsigned;
		MYSQL_TIME value_datetime;
	};
	XString<char> value_string;
	mutable XString<wchar_t> conv_buffer;

public:
	CMySQLVariant();
	CMySQLVariant(const enum_field_types type);
	CMySQLVariant(const enum_field_types type, const size_t max_len);

	CMySQLVariant(const CMySQLVariant &obj) = default;
	CMySQLVariant(CMySQLVariant &&obj) = default;
	CMySQLVariant &operator=(const CMySQLVariant &obj) = default;
	CMySQLVariant &operator=(CMySQLVariant &&obj) = default;

	inline bool IsDate() const { return IsDate(value_type); }
	inline bool IsString() const { return IsString(value_type); }
	inline bool IsDecimal() const { return IsDecimal(value_type); }
	inline bool IsEnum() const { return IsEnum(value_type); }
	inline bool IsVectorType() const { return IsVectorType(value_type); }
	inline bool IsInteger() const { return IsInteger(value_type); }
	inline bool IsNull() const { return IsNull(value_type); }

	static inline bool IsDate(const enum_field_types value_type);
	static inline bool IsString(const enum_field_types value_type);
	static inline bool IsDecimal(const enum_field_types value_type);
	static inline bool IsEnum(const enum_field_types value_type);
	static inline bool IsVectorType(const enum_field_types value_type);
	static inline bool IsInteger(const enum_field_types value_type);
	static inline bool IsNull(const enum_field_types value_type);

	inline int GetInt() const;
	inline unsigned GetUInt() const;

	inline void UpdateLength(const size_t len);
	inline const char *GetString() const;
	inline const wchar_t *GetWString() const;

	inline CDate GetDate() const;
	inline void *GetValuePtr();
	inline const void *GetValuePtr() const;
	inline size_t GetBufferSize() const;
	inline size_t GetValueLength() const;

	inline enum_field_types GetValueType() const { return value_type; }

	inline void SetInt(const int value);
	inline void SetUInt(const unsigned value);

	inline void SetDate(const unsigned day, \
						const unsigned month, \
						const unsigned year);
	inline void SetDate(const CDate &date);

	inline void SetString(const char *value);
	inline void SetString(const char *value, const size_t len);
	inline void SetString(const wchar_t *value);
	inline void SetString(const wchar_t *value, const size_t len);

	inline void SetDecimal(const char *value);
	inline void SetDecimal(const wchar_t *value);

	~CMySQLVariant();
};

//*************************************************************

bool CMySQLVariant::IsDate(const enum_field_types value_type) {

	return value_type == MYSQL_TYPE_TIMESTAMP || \
			value_type == MYSQL_TYPE_DATE;
}

bool CMySQLVariant::IsString(const enum_field_types value_type) {

	return value_type == MYSQL_TYPE_STRING || \
			value_type == MYSQL_TYPE_VAR_STRING;
}

bool CMySQLVariant::IsDecimal(const enum_field_types value_type) {

	return value_type == MYSQL_TYPE_DECIMAL || \
			value_type == MYSQL_TYPE_NEWDECIMAL;
}

bool CMySQLVariant::IsEnum(const enum_field_types value_type) {

	return value_type == MYSQL_TYPE_ENUM;
}

bool CMySQLVariant::IsVectorType(const enum_field_types value_type) {

	return IsString(value_type) || \
			IsDecimal(value_type) || \
			IsEnum(value_type);
}

bool CMySQLVariant::IsInteger(const enum_field_types value_type) {
	
	return value_type == MYSQL_TYPE_TINY || \
			value_type == MYSQL_TYPE_SHORT || \
			value_type == MYSQL_TYPE_INT24 || \
			value_type == MYSQL_TYPE_LONG || \
			value_type == MYSQL_TYPE_LONGLONG;
}

bool CMySQLVariant::IsNull(const enum_field_types value_type) {

	return value_type == MYSQL_TYPE_NULL;
}

int CMySQLVariant::GetInt() const {
	
	assert(IsInteger());
	return value_int;
}

unsigned CMySQLVariant::GetUInt() const { 
	
	assert(IsInteger());
	return value_unsigned; 
}

void CMySQLVariant::UpdateLength(const size_t len) {

	value_string.resize(len);
}

const char *CMySQLVariant::GetString() const{

	assert(IsString());
	assert(!value_string.size() || (value_string.size() && \
			value_string.c_str()[value_string.size()] == '\0'));

	conv_buffer.clear();
	return value_string.c_str();
}

const wchar_t *CMySQLVariant::GetWString() const{

	assert(IsString());
	assert(!value_string.size() || (value_string.size() && \
			value_string.c_str()[value_string.size()] == '\0'));

	return UTF8_ToUCS16(value_string, conv_buffer);
}

CDate CMySQLVariant::GetDate() const{

	assert(IsDate());
	return CDate(value_datetime.day, \
					value_datetime.month, \
					value_datetime.year);
}

void *CMySQLVariant::GetValuePtr() {
	return IsVectorType() ? (void *)value_string.c_str_writable() : \
							IsDate() ? (void *)&value_datetime : \
										(void *)&value_int;
}

const void *CMySQLVariant::GetValuePtr() const {
	return IsVectorType() ? (const void *)value_string.c_str() : \
							IsDate() ? (const void *)&value_datetime : \
										(const void *)&value_int;
}

size_t CMySQLVariant::GetBufferSize() const{

	return value_string.capacity();
}

size_t CMySQLVariant::GetValueLength() const{

	return conv_buffer.empty() ? value_string.size() : conv_buffer.size();
}

void CMySQLVariant::SetInt(const int value) {

	assert(IsInteger());
	value_int = value;
}

void CMySQLVariant::SetUInt(const unsigned value) {

	assert(IsInteger());
	value_unsigned = value;
}

void CMySQLVariant::SetDate(const unsigned day, \
							const unsigned month, \
							const unsigned year) {

	assert(IsDate());
	value_datetime.day = day;
	value_datetime.month = month;
	value_datetime.year = year;
}

void CMySQLVariant::SetDate(const CDate &date) {

	assert(IsDate());
	value_datetime.day = date.GetDay();
	value_datetime.month = date.GetMonth();
	value_datetime.year = date.GetYear();
}

void CMySQLVariant::SetString(const char *value) {

	assert(IsString());
	value_string = value;
}

void CMySQLVariant::SetString(const char *value, const size_t len){

	assert(IsString());
	value_string.assign(value, len);
}

void CMySQLVariant::SetString(const wchar_t *value) {

	assert(IsString());
	UCS16_ToUTF8(value, -1, value_string);
	if (value_string.size())
		value_string.resize(value_string.size() - 1);
}

void CMySQLVariant::SetString(const wchar_t *value, const size_t len) {

	assert(IsString());
	UCS16_ToUTF8(value, (int)len, value_string);
	if (value_string.size())
		value_string.resize(value_string.size() - 1);
}

void CMySQLVariant::SetDecimal(const char *value) {

	assert(IsDecimal());
	value_string = value;
}

void CMySQLVariant::SetDecimal(const wchar_t *value) {

	assert(IsDecimal());
	UCS16_ToUTF8(value, -1, value_string);
	if (value_string.size())
		value_string.resize(value_string.size() - 1);
}    
