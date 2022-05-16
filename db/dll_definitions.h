#pragma once

#ifdef DBLIB_IS_DLL
 #ifdef  DBLIB_DLL_EXPORTS 
  #define DBLIB_DLL_EXPORT __declspec(dllexport)
 #else
  #define DBLIB_DLL_EXPORT __declspec(dllimport)
 #endif
#else
 #define DBLIB_DLL_EXPORT
#endif
