#include <windows.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <cstdio>

//#define USE_VECTOR_SERIALIZATION

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

void ShowPathInfo(INT32 numPathArrayElements, DISPLAYCONFIG_PATH_INFO* pathInfoArray) {
	for (size_t i = 0; i < numPathArrayElements; i++)
	{
		std::wcout << "\nPATHINFO " << i << " / " << numPathArrayElements
			<< "\n\nSOURCE"
			<< "\nadapterId\t\t" << pathInfoArray[i].sourceInfo.adapterId.HighPart << " " << pathInfoArray[i].sourceInfo.adapterId.LowPart
			<< "\ncloneGroupId\t\t" << pathInfoArray[i].sourceInfo.cloneGroupId
			<< "\nid\t\t\t" << pathInfoArray[i].sourceInfo.id
			<< "\nmodeInfoIdx\t\t" << pathInfoArray[i].sourceInfo.modeInfoIdx
			<< "\nsourceModeInfoIdx\t" << pathInfoArray[i].sourceInfo.sourceModeInfoIdx
			<< "\nstatusFlags\t\t" << pathInfoArray[i].sourceInfo.statusFlags;

		std::wcout << "\n\nTARGET"
			<< "\nadapterId_H\t\t" << pathInfoArray[i].targetInfo.adapterId.HighPart
			<< "\nadapterId_L\t\t" << pathInfoArray[i].targetInfo.adapterId.LowPart
			<< "\ndesktopModeInfoIdx\t" << pathInfoArray[i].targetInfo.desktopModeInfoIdx
			<< "\nid\t\t\t" << pathInfoArray[i].targetInfo.id
			<< "\nmodeInfoIdx\t\t" << pathInfoArray[i].targetInfo.modeInfoIdx
			<< "\noutputTechnology\t" << pathInfoArray[i].targetInfo.outputTechnology
			<< "\nrefreshRate\t\t" << pathInfoArray[i].targetInfo.refreshRate.Numerator << " / " << pathInfoArray[i].targetInfo.refreshRate.Denominator
			<< "\nrotation\t\t" << pathInfoArray[i].targetInfo.rotation
			<< "\nscaling\t\t\t" << pathInfoArray[i].targetInfo.scaling
			<< "\nscanLineOrdering\t" << pathInfoArray[i].targetInfo.scanLineOrdering
			<< "\ntargetAvailable\t\t" << pathInfoArray[i].targetInfo.targetAvailable
			<< "\ntargetModeInfoIdx\t" << pathInfoArray[i].targetInfo.targetModeInfoIdx
			<< "\nstatusFlags\t\t" << pathInfoArray[i].targetInfo.statusFlags;

		std::wcout << "\n======================================================================\n";
	}

}

void ShowModeInfo(INT32 numModeArrayElements, DISPLAYCONFIG_MODE_INFO* modeInfoArray) {
	for (size_t i = 0; i < numModeArrayElements; i++)
	{
		std::wcout << "\nMODEINFO " << i << " / " << numModeArrayElements
			<< "\n"
			<< "\nadapterId\t" << modeInfoArray[i].adapterId.HighPart << " " << modeInfoArray[i].adapterId.LowPart
			<< "\nid\t\t" << modeInfoArray[i].id
			<< "\ninfoType\t" << modeInfoArray[i].infoType;

		if (modeInfoArray[i].infoType == DISPLAYCONFIG_MODE_INFO_TYPE_SOURCE) {
			std::wcout << "\n\n\tSOURCEMODE"
				<< "\nsourceMode.width\t\t" << modeInfoArray[i].sourceMode.width
				<< "\nsourceMode.height\t\t" << modeInfoArray[i].sourceMode.height
				<< "\nsourceMode.pixelFormat\t\t" << modeInfoArray[i].sourceMode.pixelFormat
				<< "\nsourceMode.position.x\t\t" << modeInfoArray[i].sourceMode.position.x
				<< "\nsourceMode.position.y\t\t" << modeInfoArray[i].sourceMode.position.y
				;
		}

		if (modeInfoArray[i].infoType == DISPLAYCONFIG_MODE_INFO_TYPE_TARGET) {
			std::wcout << "\n\n\tTARGETMODE"
				<< "\ntargetMode.activeSize.cx\t" << modeInfoArray[i].targetMode.targetVideoSignalInfo.activeSize.cx
				<< "\ntargetMode.activeSize.cy\t" << modeInfoArray[i].targetMode.targetVideoSignalInfo.activeSize.cy
				<< "\ntargetMode.totalSize.cy\t\t" << modeInfoArray[i].targetMode.targetVideoSignalInfo.totalSize.cx
				<< "\ntargetMode.totalSize.cy\t\t" << modeInfoArray[i].targetMode.targetVideoSignalInfo.totalSize.cy
				;
		}

		std::wcout << "\n======================================================================\n";
	}
}

void PrintErrorSetDisplayConfig(LONG err) {
	std::wcout << "\n!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n"
		<< (err == ERROR_SUCCESS ? "ERROR_SUCCESS" : "")
		<< (err == ERROR_INVALID_PARAMETER ? "ERROR_INVALID_PARAMETER" : "")
		<< (err == ERROR_NOT_SUPPORTED ? "ERROR_NOT_SUPPORTED" : "")
		<< (err == ERROR_ACCESS_DENIED ? "ERROR_ACCESS_DENIED" : "")
		<< (err == ERROR_GEN_FAILURE ? "ERROR_GEN_FAILURE" : "")
		<< (err == ERROR_BAD_CONFIGURATION ? "ERROR_BAD_CONFIGURATION" : "")
		<< "\n!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n"
		;
}

void TestDisplayConfiguration(BOOL showPathInfo, BOOL showModeInfo) {
	UINT32 numPathArrayElements = 0;
	UINT32 numModeArrayElements = 0;

	GetDisplayConfigBufferSizes(QDC_ONLY_ACTIVE_PATHS, &numPathArrayElements, &numModeArrayElements);

	std::vector<DISPLAYCONFIG_PATH_INFO> pathInfoArray(numPathArrayElements);
	std::vector<DISPLAYCONFIG_MODE_INFO> modeInfoArray(numModeArrayElements);

	LONG err = QueryDisplayConfig(QDC_ONLY_ACTIVE_PATHS, &numPathArrayElements, pathInfoArray.data(), &numModeArrayElements, modeInfoArray.data(), nullptr);
	if (err != ERROR_SUCCESS) {
		std::cerr << "Failed to query display configuration." << std::endl;
		return;
	}

	if (showPathInfo) {
		ShowPathInfo(numPathArrayElements, pathInfoArray.data());
	}

	if (showModeInfo) {
		ShowModeInfo(numModeArrayElements, modeInfoArray.data());
	}

	std::cout << "\napply gathered settings? y - yes:\n";
	char confirm;
	std::cin >> confirm;

	if (confirm != 'y') {
		std::wcout << "\ncanceled.\n";
		return;
	}

	// just test (SDC_VALIDATE flag)
	//err = SetDisplayConfig(numPathArrayElements, pathInfoArray.data(), numModeArrayElements, modeInfoArray.data(), SDC_VALIDATE | SDC_USE_SUPPLIED_DISPLAY_CONFIG);

	// real test. set current config and apply it... omg god please help me
	err = SetDisplayConfig(
		numPathArrayElements, 
		pathInfoArray.data(), 
		numModeArrayElements, 
		modeInfoArray.data(), 
		SDC_APPLY | SDC_USE_SUPPLIED_DISPLAY_CONFIG | SDC_SAVE_TO_DATABASE
	);

	// tested in manual mode. god it works on my machine thank you!

	PrintErrorSetDisplayConfig(err);

	//doesn't work for some reason. guess there's no coming back from now on
	//TODO: fix
	/*
	char revert;
	std::wcout << "revert back? y - yes:\n";
	std::cin >> revert;
	
	if (revert == 'y') {

		std::wcout << "\nreverting...\n";
		err = SetDisplayConfig(numPathArrayElements, pathInfoArray.data(), numModeArrayElements, modeInfoArray.data(), SDC_APPLY | SDC_USE_DATABASE_CURRENT);
		std::wcout << "\n!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n"
			<< (err == ERROR_SUCCESS ? "ERROR_SUCCESS" : "")
			<< (err == ERROR_INVALID_PARAMETER ? "ERROR_INVALID_PARAMETER" : "")
			<< (err == ERROR_NOT_SUPPORTED ? "ERROR_NOT_SUPPORTED" : "")
			<< (err == ERROR_ACCESS_DENIED ? "ERROR_ACCESS_DENIED" : "")
			<< (err == ERROR_GEN_FAILURE ? "ERROR_GEN_FAILURE" : "")
			<< (err == ERROR_BAD_CONFIGURATION ? "ERROR_BAD_CONFIGURATION" : "")
			<< "\n!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n"
			;
	}
	else {
		std::wcout << "\ndone.\n";
	}*/
}

LONG GetActiveDisplayConfig(UINT32& numPathElements, DISPLAYCONFIG_PATH_INFO* pathInfo, UINT32& numModeElements, DISPLAYCONFIG_MODE_INFO* modeInfo) {
	UINT32 numPathArrayElements = 0;
	UINT32 numModeArrayElements = 0;

	GetDisplayConfigBufferSizes(QDC_ONLY_ACTIVE_PATHS, &numPathArrayElements, &numModeArrayElements);

	//DISPLAYCONFIG_PATH_INFO* pathInfoArray = new DISPLAYCONFIG_PATH_INFO[numPathArrayElements];
	//DISPLAYCONFIG_MODE_INFO* modeInfoArray = new DISPLAYCONFIG_MODE_INFO[numModeArrayElements];
	std::vector<DISPLAYCONFIG_PATH_INFO> pathInfoArray(numPathArrayElements);
	std::vector<DISPLAYCONFIG_MODE_INFO> modeInfoArray(numModeArrayElements);

	LONG err = QueryDisplayConfig(QDC_ONLY_ACTIVE_PATHS, &numPathArrayElements, pathInfoArray.data(), &numModeArrayElements, modeInfoArray.data(), nullptr);
	if (err != ERROR_SUCCESS) {
		std::cerr << "Failed to query display configuration." << std::endl;
		return err;
	}

	pathInfo = pathInfoArray.data();
	modeInfo = modeInfoArray.data();
	numPathElements = numPathArrayElements;
	numModeElements = numModeArrayElements;
	return err;
}

struct DisplayConfig {
	UINT32 numPathArrayElements = 0;
	UINT32 numModeArrayElements = 0;
	std::vector<DISPLAYCONFIG_PATH_INFO> pathInfoArray;
	std::vector<DISPLAYCONFIG_MODE_INFO> modeInfoArray;
};

std::string GetExecutablePath() {
	char buffer[MAX_PATH];
	// Get the full path of the executable
	GetModuleFileNameA(NULL, buffer, MAX_PATH);
	std::string executablePath(buffer);
	// Return the directory of the executable
	return executablePath.substr(0, executablePath.find_last_of("\\/"));
}

void Serialize(DisplayConfig& dc, const std::string& filePath) {
	remove(filePath.c_str());

	std::ofstream outFile(filePath, std::ios::binary | std::ios::app);
	if (!outFile) {
		std::cerr << "Can't open file for writing." << std::endl;
		return;
	}

	outFile.write(reinterpret_cast<const char*>(&dc.numPathArrayElements), sizeof(dc.numPathArrayElements));
	outFile.write(reinterpret_cast<const char*>(&dc.numModeArrayElements), sizeof(dc.numModeArrayElements));
	
	for (const DISPLAYCONFIG_PATH_INFO& path : dc.pathInfoArray) {
		outFile.write(reinterpret_cast<const char*>(&path), sizeof(path));
	}

	for (const DISPLAYCONFIG_MODE_INFO& mode : dc.modeInfoArray) {
		outFile.write(reinterpret_cast<const char*>(&mode), sizeof(mode));
	}
	
	outFile.close();
	std::wcout << "file written...\n";
}

DisplayConfig DeserializeDC(const std::string& filePath) {
	DisplayConfig dc;

	std::ifstream inFile(filePath, std::ios::binary);
	if (!inFile) {
		std::cerr << "Can't open file for reading." << std::endl;
		return dc;
	}

	if (!inFile.read(reinterpret_cast<char*>(&dc.numPathArrayElements), sizeof(dc.numPathArrayElements))) {
		std::cerr << "numPAE???\n";
		return dc;
	}
	if (!inFile.read(reinterpret_cast<char*>(&dc.numModeArrayElements), sizeof(dc.numModeArrayElements))) {
		std::cerr << "numMIAE???\n";
		return dc;
	}

	for (size_t i = 0; i < dc.numPathArrayElements; i++)
	{
		DISPLAYCONFIG_PATH_INFO path;
		if (!inFile.read(reinterpret_cast<char*>(&path), sizeof(path))) {
			std::cerr << "path wrong\n";
			return dc;
		}
		dc.pathInfoArray.push_back(path);
	}

	for (size_t i = 0; i < dc.numModeArrayElements; i++)
	{
		DISPLAYCONFIG_MODE_INFO mode;
		if (!inFile.read(reinterpret_cast<char*>(&mode), sizeof(mode))) {
			std::cerr << "mode wrong\n";
			return dc;
		}
		dc.modeInfoArray.push_back(mode);
	}

	inFile.close();

	return dc;
}

LONG SerializeActiveDisplayConfig(const std::string& filePath) {
	UINT32 numPathArrayElements = 0;
	UINT32 numModeArrayElements = 0;

	GetDisplayConfigBufferSizes(QDC_ONLY_ACTIVE_PATHS, &numPathArrayElements, &numModeArrayElements);

	std::vector<DISPLAYCONFIG_PATH_INFO> pathInfoArray(numPathArrayElements);
	std::vector<DISPLAYCONFIG_MODE_INFO> modeInfoArray(numModeArrayElements);

	LONG err = QueryDisplayConfig(QDC_ONLY_ACTIVE_PATHS, &numPathArrayElements, pathInfoArray.data(), &numModeArrayElements, modeInfoArray.data(), nullptr);
	if (err != ERROR_SUCCESS) {
		std::cerr << "Failed to query display configuration." << std::endl;
		return err;
	}

	DisplayConfig dc{ numPathArrayElements, numModeArrayElements, pathInfoArray, modeInfoArray };
	
	//ShowPathInfo(dc.numPathArrayElements, dc.pathInfoArray.data());
	//ShowModeInfo(dc.numModeArrayElements, dc.modeInfoArray.data());

	Serialize(dc, filePath);
	
	return err;
}

#define DC_SHOW  0x00000001
#define DC_TEST  0x00000002
#define DC_APPLY 0x00000004

LONG DeserializeActiveDisplayConfig(const std::string& filePath, DWORD flags) {
	DisplayConfig dcOut = DeserializeDC(filePath);
	LONG err = ERROR_SUCCESS;

	if (flags & DC_SHOW) {
		ShowPathInfo(dcOut.numPathArrayElements, dcOut.pathInfoArray.data());
		ShowModeInfo(dcOut.numModeArrayElements, dcOut.modeInfoArray.data());
	}
	
	if (flags & DC_TEST) {
		std::wcout << "\n\tTESTING...\n";
		err = SetDisplayConfig(
			dcOut.numPathArrayElements,
			dcOut.pathInfoArray.data(),
			dcOut.numModeArrayElements,
			dcOut.modeInfoArray.data(),
			SDC_VALIDATE | SDC_USE_SUPPLIED_DISPLAY_CONFIG
		);
		PrintErrorSetDisplayConfig(err);
	}
	
	if (flags & DC_APPLY) {
		std::wcout << "\n\tAPPLYING...\n";
		err = SetDisplayConfig(
			dcOut.numPathArrayElements,
			dcOut.pathInfoArray.data(),
			dcOut.numModeArrayElements,
			dcOut.modeInfoArray.data(),
			SDC_APPLY | SDC_USE_SUPPLIED_DISPLAY_CONFIG | SDC_SAVE_TO_DATABASE
		);
		PrintErrorSetDisplayConfig(err);
	}

	return err;
}

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
		case 1: filePath = exePath + configsFolder + "1.bin"; DeserializeActiveDisplayConfig(filePath, DC_SHOW | DC_TEST | DC_APPLY); break;
		case 2: filePath = exePath + configsFolder + "2.bin"; DeserializeActiveDisplayConfig(filePath, DC_SHOW | DC_TEST | DC_APPLY); break;
		case 3: filePath = exePath + configsFolder + "3.bin"; DeserializeActiveDisplayConfig(filePath, DC_SHOW | DC_TEST | DC_APPLY); break;
		default:
			break;
		}
	} while (res >= 1 && res <= 3);

	//DeserializeActiveDisplayConfig(filePath, DC_SHOW | DC_TEST );
}