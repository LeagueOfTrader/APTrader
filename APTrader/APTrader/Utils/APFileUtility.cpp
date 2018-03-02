#include "APFileUtility.h"
#include <direct.h>
#include <io.h>

void APFileUtility::validPath(std::string filePath) {
	if (!existsPath(filePath)) {
		_mkdir(filePath.c_str());
	}
}

bool APFileUtility::existsPath(std::string filePath) {
	int ret = _access(filePath.c_str(), 0);

	if (0 == ret) {
		return true;
	}

	return false;
}
