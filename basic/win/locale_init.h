#include <locale.h>

inline void setLocaleToCurrOS_Locale() {

	setlocale(LC_ALL, "ukr_ukr.1251");
}