#pragma once

#ifdef USE_BASIC_LIB_AS_DLL
#ifdef  BASIC_LIB_EXPORTS 
#define BASIC_LIB_API __declspec(dllexport)
#define EXP_IF_BASIC_LIB_IS_DLL
#else
#define BASIC_LIB_API __declspec(dllimport)
#define EXP_IF_BASIC_LIB_IS_DLL extern
#endif
#else
#define BASIC_LIB_API
#define EXP_IF_BASIC_LIB_IS_DLL
#endif