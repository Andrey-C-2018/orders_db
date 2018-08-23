#include "../FileSystem.h"
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>

bool checkFileExists(const char *path) {
	
	HANDLE file = CreateFileA(path, GENERIC_READ, FILE_SHARE_READ, 0, \
								OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (file != INVALID_HANDLE_VALUE) {
		CloseHandle(file);
		return true;
	}
	return false;
}

bool checkFileExists(const wchar_t *path) {

	HANDLE file = CreateFileW(path, GENERIC_READ, FILE_SHARE_READ, 0, \
		OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (file != INVALID_HANDLE_VALUE) {
		CloseHandle(file);
		return true;
	}
	return false;
}

void getModuleFilePath(std::string &path) {
	HMODULE hModule = GetModuleHandle(nullptr);

	path.clear();
	if (hModule) {
		const size_t MAX_PATH_LEN = 256;

		path.resize(MAX_PATH_LEN + 1);
		size_t actual_size = GetModuleFileNameA(hModule, &path[0], MAX_PATH_LEN);
		path[actual_size] = '\0';
	}
}

void getModuleDirectoryPath(std::string &path) {

	getModuleFilePath(path);
	assert(path.size());

	auto slash_pos = path.rfind("\\", path.size() - 1);
	assert(slash_pos != std::string::npos);
	path.erase(slash_pos + 1, path.size() - slash_pos);
}
