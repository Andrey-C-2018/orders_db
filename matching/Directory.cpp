#include "Directory.h"
#include <assert.h>
#include <exception>

XDirectory::XDirectory() : hFind(INVALID_HANDLE_VALUE) {

	ZeroMemory(&ffd, sizeof(WIN32_FIND_DATA));
}

XDirectory::XDirectory(XDirectory &&obj) : hFind(obj.hFind) {

	memcpy_s(&ffd, sizeof(WIN32_FIND_DATA), &obj.ffd, sizeof(WIN32_FIND_DATA));

	obj.hFind = INVALID_HANDLE_VALUE;
	ZeroMemory(&obj.ffd, sizeof(WIN32_FIND_DATA));
}

XDirectory &XDirectory::operator=(XDirectory &&obj) {

	hFind = obj.hFind;
	memcpy_s(&ffd, sizeof(WIN32_FIND_DATA), &obj.ffd, sizeof(WIN32_FIND_DATA));

	obj.hFind = INVALID_HANDLE_VALUE;
	ZeroMemory(&obj.ffd, sizeof(WIN32_FIND_DATA));

	return *this;
}

bool XDirectory::getFirstFile(const wchar_t *file_name_mask) {

	assert(file_name_mask);
	close();

	hFind = FindFirstFile(file_name_mask, &ffd);

	DWORD dwError = GetLastError();
	if (INVALID_HANDLE_VALUE == hFind && dwError && dwError != ERROR_FILE_NOT_FOUND)
		throw std::exception("Cannot read the first file in the directory");

	return INVALID_HANDLE_VALUE != hFind;
}

bool XDirectory::getNextFile() {

	assert(hFind != INVALID_HANDLE_VALUE);
	BOOL success = FindNextFile(hFind, &ffd);

	DWORD dwError = GetLastError();
	if(!success && dwError && dwError != ERROR_NO_MORE_FILES)
		throw std::exception("Cannot read the next file");

	return success > 0;
}

const wchar_t *XDirectory::getFileName() const {

	assert(hFind != INVALID_HANDLE_VALUE);
	return ffd.cFileName;
}

void XDirectory::close() {

	if (hFind != INVALID_HANDLE_VALUE) {
		FindClose(hFind);
		hFind = INVALID_HANDLE_VALUE;
	}
}

XDirectory::~XDirectory() {

	close();
}