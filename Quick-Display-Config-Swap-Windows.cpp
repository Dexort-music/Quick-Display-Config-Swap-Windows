#include <windows.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <cstdio>
#include "Utils.h"
//#include "DisplayConfigTesting.h"
#include "DisplayConfig.h"
//#include "OldFunctions.h"

int main()
{
	//PrintDisplayInfo(true);
	//SerializeDevModeRegistrySettings("profile2.bin");
	//PrintDisplayInfo(true);
	//std::cout << "\n\n\n";
	//TestDisplayConfiguration(true, true);
	
	std::string exePath = GetExecutablePath();
	std::string configsFolder = "\\configs\\";
	std::string fileName = "3.bin";

	std::string filePath = exePath + configsFolder + fileName;
	
	std::wcout << filePath.c_str() << std::endl;
	
	//SerializeActiveDisplayConfig(filePath);
	
	// quick test =)
	// this works =)
	int res;
	do {
		std::wcout << "what to do?\n1/2/3/other - exit\n";
		std::cin >> res;
		switch (res)
		{
		case 1: filePath = exePath + configsFolder + "1.bin"; DeserializeAndApplyDisplayConfig(filePath, DC_SHOW | DC_TEST | DC_APPLY); break;
		case 2: filePath = exePath + configsFolder + "2.bin"; DeserializeAndApplyDisplayConfig(filePath, DC_SHOW | DC_TEST | DC_APPLY); break;
		case 3: filePath = exePath + configsFolder + "3.bin"; DeserializeAndApplyDisplayConfig(filePath, DC_SHOW | DC_TEST | DC_APPLY); break;
		default:
			break;
		}
	} while (res >= 1 && res <= 3);

	//DeserializeActiveDisplayConfig(filePath, DC_SHOW | DC_TEST );
}