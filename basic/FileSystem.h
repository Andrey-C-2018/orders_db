#pragma once
#include <string>
#include <assert.h>

bool checkFileExists(const char *path);

bool checkFileExists(const wchar_t *path);

void getModuleFilePath(std::string &path);

void getModuleDirectoryPath(std::string &path);