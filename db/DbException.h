#pragma once
#include <basic/Exception.h>

class CDbException : public XException{
public:
	enum Errors {
		E_DB_WRONG_CODE = -1, \
		E_DB_FOREIGN_KEY_DUPLICATE = 1, \
		E_DB_PARSE_ERROR = 2, \
		E_DB_WRONG_VALUE = 3
	};

	CDbException(const int err_code, const char *err_descr);
	CDbException(const int err_code, const wchar_t *err_descr);

	CDbException(const CDbException &obj) = default;
	CDbException(CDbException &&obj) = default;

	~CDbException();
};

