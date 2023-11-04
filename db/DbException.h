#pragma once
#include <basic/Exception.h>
#include "dll_definitions.h"

class DBLIB_DLL_EXPORT CDbException : public XException{
public:
	enum Errors {
		E_DB_WRONG_CODE = -1, \
		E_DB_PRIMARY_KEY_DUPLICATE = 1, \
		E_DB_FOREIGN_KEY_DUPLICATE = 2, \
		E_DB_PARSE_ERROR = 3, \
		E_DB_WRONG_VALUE = 4, \
		E_DB_NO_REF_IN_MASTER_TABLE = 5, \
		E_DB_CONNECTION_KILLED = 6, \
		E_DB_REPREP_NEEDED = 7, \
		E_DB_PARENT_NO_ROW_UPD_DEL = 8
	};

	CDbException(const int err_code, const char *err_descr);
	CDbException(const int err_code, const wchar_t *err_descr);

	CDbException(const CDbException &obj) = default;
	CDbException(CDbException &&obj) = default;

	virtual ~CDbException();
};

