#include "Utils.h"

std::wstring GetExecutablePath() {
	wchar_t buffer[MAX_PATH];
	// Get the full path of the executable
	GetModuleFileName(NULL, buffer, MAX_PATH);
	std::wstring executablePath(buffer);
	// Return the directory of the executable

	return executablePath.substr(0, executablePath.find_last_of(L"\\/"));
}