#include <locale.h>

inline const char *getOS_Locale() {

	return "ukr_ukr.1251";
}

inline void setLocaleToCurrOS_Locale() {

	setlocale(LC_ALL, getOS_Locale());
}