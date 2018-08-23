#pragma once
#include <vector>
#include <Windows.h>
#include "../Exception.h"
#include "../Process.h"

int exec(const char *exe_path, const char *cmd_line, const char *working_dir) {

	STARTUPINFOA si;
	PROCESS_INFORMATION pi;

	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	ZeroMemory(&pi, sizeof(pi));

	std::vector<char> cmd_line_rw;
	size_t cmd_line_size = strlen(cmd_line);
	cmd_line_rw.resize(cmd_line_size + 1);
	strncpy(&cmd_line_rw[0], cmd_line, cmd_line_size);

	// Start the child process. 
	if (!CreateProcessA(exe_path, &cmd_line_rw[0], // Command line
						0, // Process handle not inheritable
						0, // Thread handle not inheritable
						FALSE, // Set handle inheritance to FALSE
						0, // No creation flags
						0, // Use parent's environment block
						working_dir, 
						&si, // Pointer to STARTUPINFO structure
						&pi) // Pointer to PROCESS_INFORMATION structure
		){
		XException e(0, "CreateProcess failed: last error code = ");
		e << GetLastError();
		throw e;
	}

	// Wait until child process exits.
	WaitForSingleObject(pi.hProcess, INFINITE);

	DWORD exit_code = 0;
	if (FALSE == GetExitCodeProcess(pi.hProcess, &exit_code)) {
		XException e(0, "GetExitCodeProcess failed: last error code = ");
		e << GetLastError();
		throw e;
	}
	assert(STILL_ACTIVE != exit_code);

	// Close process and thread handles. 
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);

	return exit_code;
}