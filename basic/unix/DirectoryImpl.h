#pragma once
#include <string>
#include <dirent.h>
#include "../tstring.h"

class XDirectory {
	DIR *dir;
	struct dirent *dir_entry;
	std::string path, fileext;
	mutable std::wstring filename_buffer;

	void open(const wchar_t *path_mask_w);
	void open(const char *path_mask);
	bool getFirstFile();
        bool getFile();
	const char *getFileName(char) const;
	const wchar_t *getFileName(wchar_t) const;

public:
	XDirectory();

	XDirectory(const XDirectory &obj) = delete;
	XDirectory(XDirectory &&obj);
	XDirectory &operator=(const XDirectory &obj) = delete;
	XDirectory &operator=(XDirectory &&obj);

	inline bool getFirstFile(const wchar_t *file_name_mask);
	inline bool getFirstFile(const char *file_name_mask);
	bool getNextFile();
	inline const Tchar *getFileName() const;

	void close();
	virtual ~XDirectory();
};

//*******************************************

bool XDirectory::getFirstFile(const wchar_t *path_mask) {

	open(path_mask);
	return getFirstFile();
}

bool XDirectory::getFirstFile(const char *path_mask) {

	open(path_mask);
	return getFirstFile();
}

const Tchar *XDirectory::getFileName() const {

	return getFileName(Tchar());
}
