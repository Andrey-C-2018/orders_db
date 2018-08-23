#pragma once
#include <iostream>
#include <fstream>

#ifdef UNICODE
	#define Tcout std::wcout
	#define Tcin std::wcin
	#define Tcerr std::wcerr
	typedef std::wfstream Tfstream;
#else
	#define Tcout std::cout
	#define Tcin std::cin
	#define Tcerr std::cerr
	typedef std::fstream Tfstream;
#endif