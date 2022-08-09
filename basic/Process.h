#pragma once
#include <string>
#include "dll_definitions.h"

void BASIC_DLL_EXPORT getUserHomeDir(std::string &var_value);

int BASIC_DLL_EXPORT exec(const char *exe_path, \
							const char *cmd_line, \
							const char *working_dir, const bool wait);
