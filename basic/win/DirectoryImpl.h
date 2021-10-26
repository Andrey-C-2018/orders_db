#pragma once
#include <windows.h>

class XDirectory {
	WIN32_FIND_DATA ffd;
	HANDLE hFind;

public:
	XDirectory();

	XDirectory(const XDirectory &obj) = delete;
	XDirectory(XDirectory &&obj);
	XDirectory &operator=(const XDirectory &obj) = delete;
	XDirectory &operator=(XDirectory &&obj);

	bool getFirstFile(const wchar_t *file_name_mask);
	bool getNextFile();
	const wchar_t *getFileName() const;

	void close();
	virtual ~XDirectory();
};
