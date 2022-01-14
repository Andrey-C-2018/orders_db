#pragma once
#include <iostream>
#include <fstream>

#ifdef UNICODE
	#define Tcout std::wcout
	#define Tcin std::wcin
	#define Tcerr std::wcerr
	typedef std::wfstream Tfstream;
	typedef std::wifstream Tifstream;
	typedef std::wofstream Tofstream;
#else
	#define Tcout std::cout
	#define Tcin std::cin
	#define Tcerr std::cerr
	typedef std::fstream Tfstream;
	typedef std::ifstream Tifstream;
	typedef std::ofstream Tofstream;
#endif