#include <assert.h>
#include "CsvTable.h"

CCsvTableException::CCsvTableException(const int err_code, \
										const Tchar *err_descr) : \
										XException(err_code, err_descr) { }

CCsvTableException::CCsvTableException(const CCsvTableException &obj) : \
										XException(obj) { }

CCsvTableException::~CCsvTableException() { }
