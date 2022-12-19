#pragma once
#include <basic/Exception.h>
#include <basic/PropertiesFile.h>

class CFieldsListException : public XException {
public:
	enum {
		E_WRONG_FORMAT = 1, \
		E_DUPLICATE = 2, \
		E_OUT_OF_RANGE = 3, \
		E_NOT_ALL_USED = 4, \
		E_TOO_LARGE = 5, \
		E_NOT_INTEGER = 6, \
		E_PROPERTY_NOT_FOUND = 7
	};
	CFieldsListException(const int err_code, const Tchar *err_descr);
	CFieldsListException(const CFieldsListException &obj);
	CFieldsListException(CFieldsListException &&obj) = default;
	virtual ~CFieldsListException();
};

class FieldsList final {
	enum {
		FIELDS_COUNT = 54
	};

	size_t indexes[FIELDS_COUNT];
	static size_t getFieldIndex(std::string field_indexes_str, \
								size_t first, size_t last);

public:
	FieldsList();

	FieldsList(const FieldsList &obj) = delete;
	FieldsList(FieldsList &&obj) = default;
	FieldsList &operator=(const FieldsList &obj) = delete;
	FieldsList &operator=(FieldsList &&obj) = default;

	void open(const char *field_indexes_str);
	void open(const IProperties &props);
	std::string getFieldsList() const;
};
