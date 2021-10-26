#pragma once
#include <vector>
#include <basic/tstring.h>
#include <basic/TextConv.h>

inline const char *convertIfNecessary(const char *text, std::string &buffer) {

       assert(text);
       buffer = text;
       return &buffer[0];
}