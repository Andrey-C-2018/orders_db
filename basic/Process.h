#pragma once
#include <string>

void getUserHomeDir(std::string &var_value);

int exec(const char *exe_path, \
			const char *cmd_line, \
			const char *working_dir, const bool wait);
