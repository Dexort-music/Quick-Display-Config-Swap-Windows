#include "DisplayConfig.h"

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

void Serialize(DisplayConfig& dc, const std::wstring& filePath) {
	DeleteFile(filePath.c_str());

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

DisplayConfig Deserialize(const std::wstring& filePath) {
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

LONG SerializeActiveDisplayConfig(const std::wstring& filePath) {
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

LONG DeserializeAndApplyDisplayConfig(const std::wstring& filePath, DWORD flags) {
	DisplayConfig dcOut = Deserialize(filePath);
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
