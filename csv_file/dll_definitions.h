#pragma once

#ifdef CSVTABLE_IS_DLL
 #ifdef  CSVTABLE_DLL_EXPORTS 
  #define CSVTABLE_DLL_EXPORT __declspec(dllexport)
 #else
  #define CSVTABLE_DLL_EXPORT __declspec(dllimport)
 #endif
#else
 #define CSVTABLE_DLL_EXPORT
#endif
