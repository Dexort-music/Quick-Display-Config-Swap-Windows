#include "Utils.h"

std::string GetExecutablePath() {
	char buffer[MAX_PATH];
	// Get the full path of the executable
	GetModuleFileNameA(NULL, buffer, MAX_PATH);
	std::string executablePath(buffer);
	// Return the directory of the executable
	// test comment
	return executablePath.substr(0, executablePath.find_last_of("\\/"));
}