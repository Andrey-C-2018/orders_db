#pragma once
#include <iostream>

#ifdef UNICODE
	#define Tcout std::wcout
	#define Tcin std::wcin
	#define Tcerr std::wcerr
#else
	#define Tcout std::cout
	#define Tcin std::cin
	#define Tcerr std::cerr
#endif