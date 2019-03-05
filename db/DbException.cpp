#include "DbException.h"

CDbException::CDbException(const int err_code, const char *err_descr) : \
							XException(err_code, err_descr){ }

CDbException::CDbException(const int err_code, const wchar_t *err_descr) : \
							XException(err_code, err_descr) { }

CDbException::~CDbException() { }
