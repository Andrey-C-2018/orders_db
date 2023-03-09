#pragma once
#include <string>
#include "dll_definitions.h"

bool BASIC_DLL_EXPORT checkFileExists(const char *path);

bool BASIC_DLL_EXPORT checkFileExists(const wchar_t *path);

void BASIC_DLL_EXPORT getModuleFilePath(std::string &path);

void BASIC_DLL_EXPORT getModuleDirectoryPath(std::string &path);