#pragma once

template <typename Tchar_> \
struct CDbField {
	const Tchar_ *field_name;
	const Tchar_ *table_name;
	bool is_primary_key;
};

struct IDbResultSetMetadata {
	virtual size_t getFieldsCount() const = 0;
	virtual CDbField<char> getField(const size_t field) const = 0;
	virtual CDbField<wchar_t> getFieldW(const size_t field) const = 0;
	virtual ~IDbResultSetMetadata(){ }
};