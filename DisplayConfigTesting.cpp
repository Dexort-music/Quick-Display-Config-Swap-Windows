#include "DisplayConfigTesting.h"

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