#include <fcntl.h>
#include <unistd.h>
#include <cassert>
#include "../FileSystem.h"

bool checkFileExists(const char *path) {

	assert(path);
	int file = open(path, O_RDONLY | O_NONBLOCK, 0);
	bool exists = file >= 0;
	close(file);

	return exists;
}

bool checkFileExists(const wchar_t *path) {

	return false;
}

void getModuleFilePath(std::string &path) {

}

void getModuleDirectoryPath(std::string &path) {

}
