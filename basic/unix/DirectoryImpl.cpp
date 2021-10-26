#include <assert.h>
#include <errno.h>
#include "../Exception.h"
#include "../TextConv.h"
#include "DirectoryImpl.h"

XDirectory::XDirectory() : dir(nullptr) { }

XDirectory::XDirectory(XDirectory &&obj) : dir(obj.dir), dir_entry(obj.dir_entry), \
						path(std::move(obj.path)), \
						fileext(std::move(obj.fileext)), \
						filename_buffer(std::move(obj.filename_buffer)) {

	obj.dir = nullptr;
	obj.dir_entry = nullptr;
}

XDirectory &XDirectory::operator=(XDirectory &&obj) {

	dir = obj.dir;
	dir_entry = obj.dir_entry;
	path = std::move(obj.path);
	fileext = std::move(obj.fileext);
	filename_buffer = std::move(obj.filename_buffer);

	obj.dir = nullptr;
	obj.dir_entry = nullptr;
	return *this;
}

void XDirectory::open(const wchar_t *path_mask_w) {

	assert(path_mask_w);
	close();

	std::string buffer;
	UCS16_ToUTF8(path_mask_w, -1, buffer);
	open(buffer.c_str());
}

void XDirectory::open(const char *path_mask) {

	assert(path_mask);

	auto p_slash = strrchr(path_mask, '/');
	if(!p_slash) 
		path = path_mask;
	else {
		auto p = p_slash + 1;
		bool templ_found = *p != '\0' && !strncmp(p, "*.", 2);
		if(templ_found && *(p + 2) != '\0' && !strchr("()-+/*[]&^%!`", *(p + 2)))
			fileext = (p + 2);

		if(*p != '\0' && (*p != '*' || (*p == '*' && *(p + 1) != '\0')) && fileext.empty())
			path = path_mask;
		else
			path.assign(path_mask, p_slash);
	}

	dir = opendir(path.c_str());
	if(!dir) {
		XException e(0, "Cannot open the directory '");
		e << path << "'";
		throw e;
	}
}

bool XDirectory::getFile() {

	assert(dir);
	errno = 0;
	dir_entry = readdir(dir);
	if(errno){
		XException e(0, "Cannot read a file from the directory '");
		e << path << "'";
		throw e;
	}

	return dir_entry;
}

bool XDirectory::getFirstFile() {

	assert(dir != nullptr);

	bool files_exist = getFile();
	while(files_exist && !(strcmp(dir_entry->d_name, ".") && strcmp(dir_entry->d_name, "..")))
		files_exist = getFile();

	bool ext_empty = fileext.empty();
	while(files_exist && !ext_empty && \
			!endsWith(dir_entry->d_name, fileext.c_str(), fileext.size()))
		files_exist = getFile();

	filename_buffer.clear();
	return files_exist;
}

bool XDirectory::getNextFile() {

	bool files_exist = getFile();
	bool ext_empty = fileext.empty();
	while(files_exist && !ext_empty && \
			!endsWith(dir_entry->d_name, fileext.c_str(), fileext.size()))
		files_exist = getFile();

	filename_buffer.clear();
	return files_exist;
}

const wchar_t *XDirectory::getFileName(wchar_t) const {

	if(dir_entry) {
		if(filename_buffer.empty())
			UTF8_ToUCS16(dir_entry->d_name, -1, filename_buffer);
		return filename_buffer.c_str();
	}
	return nullptr;
}

const char *XDirectory::getFileName(char) const {

	return dir_entry ? dir_entry->d_name : nullptr;
}

void XDirectory::close() {

	if (dir) closedir(dir);
	path.clear();
	fileext.clear();
	filename_buffer.clear();
}

XDirectory::~XDirectory() {

	close();
}
