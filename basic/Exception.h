#pragma once
#include "tstring.h"
#include "dll_definitions.h"

class BASIC_DLL_EXPORT XException{
	int err_code;

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4251)
#endif
	Tstring err_msg;
#ifdef _MSC_VER
#pragma warning(pop)
#endif

public:
	XException(const int err_code, const char *err_descr);
	XException(const int err_code, const wchar_t *err_descr);
	XException(const int err_code, const std::string &err_descr);
	XException(const int err_code, const std::wstring &err_descr);
	XException(const int err_code, const size_t reserved_size);

	XException(const XException &obj);
	XException(XException &&obj) noexcept = default;
	XException &operator=(const XException &obj) = delete;
	XException &operator=(XException &&obj) noexcept = default;

	const Tchar* what() const noexcept;
	int GetErrorCode() const noexcept;

	XException &operator<<(const char *new_data);
	XException &operator<<(const wchar_t *new_data);
	XException &operator<<(const std::string &new_data);
	XException &operator<<(const std::wstring &new_data);
	XException &operator<<(const int number);
#if _WIN64
	XException &operator<<(const unsigned number);
#endif
	XException &operator<<(const size_t number);

	virtual ~XException();
};
