#pragma once
#include <Windows.h>
#include <iostream>
#include <vector>

void ShowPathInfo(INT32 numPathArrayElements, DISPLAYCONFIG_PATH_INFO* pathInfoArray);

void ShowModeInfo(INT32 numModeArrayElements, DISPLAYCONFIG_MODE_INFO* modeInfoArray);

void PrintErrorSetDisplayConfig(LONG err);

void TestDisplayConfiguration(BOOL showPathInfo, BOOL showModeInfo);