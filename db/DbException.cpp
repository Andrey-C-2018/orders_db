#include "DbException.h"

CDbException::CDbException(const int err_code, const Tchar *err_descr) : \
							XException(err_code, err_descr), db_error_code(0){ }

CDbException::CDbException(const int err_code, const int db_error_code_, \
							const char *err_descr) : \
							XException(err_code, err_descr), \
							db_error_code(db_error_code_){ }

CDbException::~CDbException() { }
