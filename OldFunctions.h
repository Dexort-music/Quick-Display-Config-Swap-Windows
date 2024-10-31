#pragma once
#include <iostream>

//#define USE_VECTOR_SERIALIZATION

template<typename T>
void SerializeToBinary(const std::string& filename, T ds) {
	std::ofstream outFile(filename, std::ios::binary | std::ios::app);
	if (!outFile) {
		std::cerr << "Can't open file for writing." << std::endl;
		return;
	}

	outFile.write(reinterpret_cast<const char*>(&ds), sizeof(ds));
	outFile.close();
	std::wcout << "file written...\n";
}

void PrintDevmodeInfo(DEVMODE& devmode) {
	std::wcout <<
		"\ndmSize\t\t\t" << devmode.dmSize <<
		"\ndmDeviceName\t\t" << devmode.dmDeviceName <<
		"\ndmBitsPerPel\t\t" << devmode.dmBitsPerPel <<
		//"\ndmCollate\t" << devmode.dmCollate <<
		//"\ndmColor\t" << devmode.dmColor <<
		//"\ndmCopies\t" << devmode.dmCopies <<
		//"\ndmDefaultSource\t" << devmode.dmDefaultSource <<
		//"\ndmDisplayFixedOutput\t" << devmode.dmDisplayFixedOutput <<
		//"\ndmDisplayFlags\t" << devmode.dmDisplayFlags <<
		"\ndmDisplayFrequency\t" << devmode.dmDisplayFrequency <<
		"\ndmDisplayOrientation\t" << devmode.dmDisplayOrientation <<
		"\ndmFields\t\t" << devmode.dmFields <<
		"\ndmMediaType\t\t" << devmode.dmMediaType <<
		"\ndmOrientation\t\t" << devmode.dmOrientation <<
		//"\ndmPanningWidth\t" << devmode.dmPanningWidth <<
		//"\ndmPanningHeight\t" << devmode.dmPanningHeight <<
		"\ndmPelsWidth\t\t" << devmode.dmPelsWidth <<
		"\ndmPelsHeight\t\t" << devmode.dmPelsHeight <<
		"\ndmPosition.x\t\t" << devmode.dmPosition.x <<
		"\ndmPosition.y\t\t" << devmode.dmPosition.y <<
		//"\ndmScale\t" << devmode.dmScale <<
		//"\ndmDuplex\t" << devmode.dmDuplex <<
		//"\ndmYResolution\t" << devmode.dmYResolution <<
		"\n====================================================================================================\n";
	;
}

struct DisplaySettings {
	DISPLAY_DEVICE displayDevice;
	DEVMODE devMode;
};

void PrintDisplayInfo(bool checkRegistry) {
	DISPLAY_DEVICE displayDevice;
	displayDevice.cb = sizeof(DISPLAY_DEVICE);

	DWORD deviceIndex = 0;

	while (EnumDisplayDevices(NULL, deviceIndex, &displayDevice, 0)) {
		DWORD sfs = displayDevice.StateFlags;
		std::wcout
			<< "\n####################################################################################################"
			<< "\nDisplay\t\t" << deviceIndex << ":"
			<< "\nName:\t\t" << displayDevice.DeviceName
			<< "\nID:\t\t" << displayDevice.DeviceID
			<< "\nKey:\t\t" << displayDevice.DeviceKey
			<< "\nString:\t\t" << displayDevice.DeviceString
			<< "\nFlags:\t\t" << displayDevice.StateFlags << "\n"
			<< ((sfs & DISPLAY_DEVICE_ATTACHED_TO_DESKTOP) ? "\tDEVICE_ATTACHED_TO_DESKTOP\n" : "")
			<< ((sfs & DISPLAY_DEVICE_MIRRORING_DRIVER) ? "\tDISPLAY_DEVICE_MIRRORING_DRIVER\n" : "")
			<< ((sfs & DISPLAY_DEVICE_MODESPRUNED) ? "\tDISPLAY_DEVICE_MODESPRUNED\n" : "")
			<< ((sfs & DISPLAY_DEVICE_PRIMARY_DEVICE) ? "\tDISPLAY_DEVICE_PRIMARY_DEVICE\n" : "")
			<< ((sfs & DISPLAY_DEVICE_REMOVABLE) ? "\tDISPLAY_DEVICE_REMOVABLE\n" : "")
			<< ((sfs & DISPLAY_DEVICE_VGA_COMPATIBLE) ? "\tDISPLAY_DEVICE_VGA_COMPATIBLE\n" : "")
			<< "====================================================================================================\n";

		DEVMODE devMode;
		ZeroMemory(&devMode, sizeof(devMode));
		devMode.dmSize = sizeof(DEVMODE);
		if (EnumDisplaySettings(displayDevice.DeviceName, (checkRegistry ? ENUM_REGISTRY_SETTINGS : ENUM_CURRENT_SETTINGS), &devMode)) {
			PrintDevmodeInfo(devMode);
		}
		else {
			std::wcout << "\nCan't retrieve display settings...\n";
		}

		deviceIndex++;
	}
}

void Serialize(DEVMODE& devMode, const std::string& filename) {
	std::vector<DisplaySettings> allDSettings;

	std::ofstream outFile(filename, std::ios::binary | std::ios::app);
	if (!outFile) {
		std::cerr << "Can't open file for writing." << std::endl;
		return;
	}

	outFile.write(reinterpret_cast<const char*>(&devMode), sizeof(devMode));
	outFile.close();
	std::wcout << "file written...\n";
}

std::vector<DEVMODE> Deserialize(const std::string& filename) {
	std::vector<DEVMODE> devModes;

	std::ifstream inFile(filename, std::ios::binary);
	if (!inFile) {
		std::cerr << "Can't open file for reading." << std::endl;
		return devModes;
	}

	DEVMODE devMode;

	while (inFile.read(reinterpret_cast<char*>(&devMode), sizeof(devMode))) {
		devModes.push_back(devMode);
	}
	inFile.close();

	return devModes;
}

void SerializeDisplaySettingsVector(std::vector<DisplaySettings> allDSettings, const std::string& filename) {
	std::ofstream outFile(filename, std::ios::binary);
	if (!outFile) {
		std::cerr << "Can't open file for writing." << std::endl;
		return;
	}
	outFile.write(reinterpret_cast<const char*>(&allDSettings), sizeof(allDSettings));
	outFile.close();
	std::wcout << "file written...\n";
}

std::vector<DisplaySettings> DeserializeDisplaySettingsVector(const std::string& filename) {
	std::vector<DisplaySettings> allDSettings;

	std::ifstream inFile(filename, std::ios::binary);
	if (!inFile) {
		std::cerr << "Can't open file for reading." << std::endl;
		return allDSettings;
	}

	inFile.read(reinterpret_cast<char*>(&allDSettings), sizeof(allDSettings));
	inFile.close();

	return allDSettings;
}

void SerializeDisplaySettings(DisplaySettings& ds, const std::string& filename) {
	std::ofstream outFile(filename, std::ios::binary | std::ios::app);
	if (!outFile) {
		std::cerr << "Can't open file for writing." << std::endl;
		return;
	}

	outFile.write(reinterpret_cast<const char*>(&ds), sizeof(ds));
	outFile.close();
	std::wcout << "file written...\n";
}

std::vector<DisplaySettings> DeserializeDisplaySettings(const std::string& filename) {
	std::vector<DisplaySettings> allDS;

	DisplaySettings ds;

	ds.devMode.dmSize = sizeof(ds.devMode);
	ds.displayDevice.cb = sizeof(ds.displayDevice);

	std::ifstream inFile(filename, std::ios::binary);
	if (!inFile) {
		std::cerr << "Can't open file for reading." << std::endl;
		return allDS;
	}

	while (inFile.read(reinterpret_cast<char*>(&ds), sizeof(ds))) {
		allDS.push_back(ds);
	}
	inFile.close();

	return allDS;
}

void SerializeDevModeRegistrySettings(const std::string& filename) {
	std::vector<DisplaySettings> allDSettings;

	DISPLAY_DEVICE displayDevice;
	displayDevice.cb = sizeof(DISPLAY_DEVICE);

	DWORD deviceIndex = 0;

	remove(filename.c_str()); // need to delete previous file for convinience (cause we write with append)

	while (EnumDisplayDevices(NULL, deviceIndex, &displayDevice, 0)) {

		DEVMODE devMode;
		ZeroMemory(&devMode, sizeof(devMode));
		devMode.dmSize = sizeof(DEVMODE);
		if (EnumDisplaySettings(displayDevice.DeviceName, ENUM_REGISTRY_SETTINGS, &devMode)) {

#ifdef USE_VECTOR_SERIALIZATION
			allDSettings.push_back({ displayDevice, devMode });
#else
			//Serialize(devMode, "registryConfig.bin");
			DisplaySettings ds{ displayDevice, devMode };
			SerializeDisplaySettings(ds, filename);
#endif

		}
		deviceIndex++;
	}

#ifdef USE_VECTOR_SERIALIZATION
	SerializeDisplaySettingsVector(allDSettings, filename);
#endif

}

void DeSerializeDevModeRegistrySettings(const std::string& filename) {
	std::vector<DisplaySettings> allDS = DeserializeDisplaySettings(filename);
	for (DisplaySettings& ds : allDS) {
		std::wcout << "\nDISPLAY\n" << ds.displayDevice.DeviceName << "\n";
		PrintDevmodeInfo(ds.devMode);

		// reset
		//ChangeDisplaySettingsEx(ds.displayDevice.DeviceName, NULL, NULL, 0, NULL);

		// real deal
		//LONG result = ChangeDisplaySettingsEx(ds.displayDevice.DeviceName, &(ds.devMode), NULL, CDS_UPDATEREGISTRY | CDS_GLOBAL);

		// testing
		LONG result = ChangeDisplaySettingsEx(ds.displayDevice.DeviceName, &(ds.devMode), NULL, CDS_TEST, NULL);

		std::wcout << (result == DISP_CHANGE_SUCCESSFUL ? "DISP_CHANGE_SUCCESSFUL\n" : "")
			<< (result == DISP_CHANGE_BADDUALVIEW ? "DISP_CHANGE_BADDUALVIEW\n" : "")
			<< (result == DISP_CHANGE_BADFLAGS ? "DISP_CHANGE_BADFLAGS\n" : "")
			<< (result == DISP_CHANGE_BADMODE ? "DISP_CHANGE_BADMODE\n" : "")
			<< (result == DISP_CHANGE_FAILED ? "DISP_CHANGE_FAILED\n" : "")
			<< (result == DISP_CHANGE_NOTUPDATED ? "DISP_CHANGE_NOTUPDATED\n" : "")
			<< (result == DISP_CHANGE_RESTART ? "DISP_CHANGE_RESTART\n" : "")
			<< "====================================================================================================\n\n";
	}
}
