#pragma once

#ifdef DATELIB_IS_DLL
 #ifdef  DATELIB_DLL_EXPORTS 
  #define DATELIB_DLL_EXPORT __declspec(dllexport)
 #else
  #define DATELIB_DLL_EXPORT __declspec(dllimport)
 #endif
#else
 #define DATELIB_DLL_EXPORT
#endif
