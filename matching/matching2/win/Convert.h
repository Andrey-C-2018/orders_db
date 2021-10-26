#pragma once
#include <string>
#include <vector>

inline const char *convertIfNecessary(const char *value, std::string &buffer) {

       buffer = value;
       return buffer.c_str();
}

inline const char *convertIfNecessary(const char *value, std::vector<char> &buffer) {

       size_t value_size = strlen(value);
       buffer.resize(value_size + 1);

       strncpy(&buffer[0], value, value_size);
       buffer[value_size] = '\0';
       return &buffer[0];
}