#pragma once
#include "tstring.h"

#ifdef _MSC_VER
constexpr const Tchar PATH_SLASH = _T('\\');
#else
constexpr const Tchar PATH_SLASH = _T('/');
#endif