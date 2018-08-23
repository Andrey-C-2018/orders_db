#pragma once
#include <basic/Exception.h>

class CDbException : public XException{
	int db_error_code;
public:
	enum Errors {
		E_DB_WRONG_CODE = -1, \
		E_DB_FOREIGN_KEY_DUPLICATE = 1, \
		E_DB_PARSE_ERROR = 2
	};

	CDbException(const int err_code, const Tchar *err_descr);
	CDbException(const int err_code, const int db_error_code_, \
					const char *err_descr);

	CDbException(const CDbException &obj) = default;
	CDbException(CDbException &&obj) = default;

	inline int GetDbErrorCode() const { return db_error_code; }

	~CDbException();
};

