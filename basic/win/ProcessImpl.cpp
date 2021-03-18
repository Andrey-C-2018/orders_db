#pragma once
#include <vector>
#include <Windows.h>
#include "../Exception.h"
#include "../Process.h"

void getUserHomeDir(std::string &var_value) {

	char buffer[MAX_PATH];
	const DWORD var_size = GetEnvironmentVariableA("USERPROFILE", buffer, MAX_PATH);

	var_value = buffer;
}

int exec(const char *exe_path, const char *cmd_line, \
		const char *working_dir, const bool wait) {

	STARTUPINFOA si;
	PROCESS_INFORMATION pi;

	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	ZeroMemory(&pi, sizeof(pi));

	std::vector<char> cmd_line_rw;
	size_t cmd_line_size = strlen(cmd_line);
	cmd_line_rw.resize(cmd_line_size + 1);

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4996)
#endif
	strncpy(&cmd_line_rw[0], cmd_line, cmd_line_size);
#ifdef _MSC_VER
#pragma warning(pop)
#endif

	// Start the child process. 
	if (!CreateProcessA(exe_path, &cmd_line_rw[0], // Command line
						0, // Process handle not inheritable
						0, // Thread handle not inheritable
						FALSE, // Set handle inheritance to FALSE
						NORMAL_PRIORITY_CLASS | CREATE_NEW_CONSOLE, // create as detached
						0, // Use parent's environment block
						working_dir, 
						&si, // Pointer to STARTUPINFO structure
						&pi) // Pointer to PROCESS_INFORMATION structure
		){
		XException e(0, "CreateProcess failed: last error code = ");
		e << (size_t)GetLastError();
		throw e;
	}

	DWORD exit_code = 0;
	if (wait) {
		WaitForSingleObject(pi.hProcess, INFINITE);

		if (FALSE == GetExitCodeProcess(pi.hProcess, &exit_code)) {
			XException e(0, "GetExitCodeProcess failed: last error code = ");
			e << (size_t)GetLastError();
			throw e;
		}
		assert(STILL_ACTIVE != exit_code);
	}

	// Close process and thread handles. 
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);

	return exit_code;
}