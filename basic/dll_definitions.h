#pragma once

#ifdef BASIC_IS_DLL
 #ifdef  BASIC_DLL_EXPORTS 
  #define BASIC_DLL_EXPORT __declspec(dllexport)
 #else
  #define BASIC_DLL_EXPORT __declspec(dllimport)
 #endif
#else
 #define BASIC_DLL_EXPORT
#endif