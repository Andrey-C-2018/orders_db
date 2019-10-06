#pragma once

#ifdef _MSC_VER
#include "win/locale_init.h"
#else
#include "unix/locale_init.h"
#endif

inline void setLocaleToCurrOS_Locale() {

	setlocale(LC_ALL, getOS_Locale());
}