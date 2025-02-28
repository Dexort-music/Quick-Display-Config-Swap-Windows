#pragma once
#include <Windows.h>
#include <iostream>
#include <fstream>
#include <vector>
#include "DisplayConfigTesting.h"

#define DC_SHOW  0x00000001
#define DC_TEST  0x00000002
#define DC_APPLY 0x00000004

struct DisplayConfig {
	UINT32 numPathArrayElements = 0;
	UINT32 numModeArrayElements = 0;
	std::vector<DISPLAYCONFIG_PATH_INFO> pathInfoArray;
	std::vector<DISPLAYCONFIG_MODE_INFO> modeInfoArray;
};

LONG GetActiveDisplayConfig(
	UINT32& numPathElements, 
	DISPLAYCONFIG_PATH_INFO* pathInfo, 
	UINT32& numModeElements, 
	DISPLAYCONFIG_MODE_INFO* modeInfo
);

void Serialize(DisplayConfig& dc, const std::wstring& filePath);

DisplayConfig Deserialize(const std::wstring& filePath);

LONG SerializeActiveDisplayConfig(const std::wstring& filePath);

LONG DeserializeAndApplyDisplayConfig(const std::wstring& filePath, DWORD flags);