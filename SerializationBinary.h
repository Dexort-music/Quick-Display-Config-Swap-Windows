#pragma once
#include <iostream>

template<typename T>
void SerializeToBinary(const std::string& filename, T) {
	std::ofstream outFile(filename, std::ios::binary | std::ios::app);
	if (!outFile) {
		std::cerr << "Can't open file for writing." << std::endl;
		return;
	}

	outFile.write(reinterpret_cast<const char*>(&ds), sizeof(ds));
	outFile.close();
	std::wcout << "file written...\n";
}