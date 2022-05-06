#pragma once
#include <fstream>

#ifdef UNICODE
typedef std::wfstream Tfstream;
typedef std::wifstream Tifstream;
typedef std::wofstream Tofstream;
#else
typedef std::fstream Tfstream;
typedef std::ifstream Tifstream;
typedef std::ofstream Tofstream;
#endif