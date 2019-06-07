#include <locale.h>

inline void setLocaleToCurrOS_Locale() {

	setlocale(LC_ALL, "uk_UA.UTF-8");
}