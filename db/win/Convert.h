#pragma once
#include <basic/TextConv.h>

inline const char *convertIfNecessary(const wchar_t *value, std::string &buffer) {

       return UCS16_ToUTF8(value, -1, buffer);
}
