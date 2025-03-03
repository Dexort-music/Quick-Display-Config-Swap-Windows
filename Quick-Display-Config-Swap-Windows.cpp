#include <windows.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <cstdio>
#include <shellapi.h>
#include <commdlg.h>

#include "Utils.h"
#include "DisplayConfig.h"

#define WM_TRAYICON (WM_USER + 1)
#define ID_TRAY_EXIT 1002
#define ID_TRAY_SAVE_CURRENT 1003
#define ID_TRAY_OPEN_FOLDER 1004

const UINT_PTR BASE_ID = 10;

NOTIFYICONDATA nid;
HMENU hTrayMenu = NULL;

std::vector<std::wstring> configFileNames;

// Function to initialize the tray icon
void InitTrayIcon(HWND hWnd) {
    nid.cbSize = sizeof(NOTIFYICONDATA);
    nid.hWnd = hWnd;
    nid.uID = 1;
    nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP | NIF_INFO;
    nid.uCallbackMessage = WM_TRAYICON;
    nid.hIcon = LoadIcon(NULL, IDI_APPLICATION); // Default application icon
    wcscpy_s(nid.szTip, L"QDCSWAP");

    Shell_NotifyIcon(NIM_ADD, &nid);
}

void UpdateTrayMenu() {
    if (hTrayMenu) {
        DestroyMenu(hTrayMenu);
    }

    hTrayMenu = CreatePopupMenu();

    std::wstring fpath = GetExecutablePath() + L"/configs/*.bin";

    WIN32_FIND_DATA findFileData;
    HANDLE hFind = FindFirstFile(fpath.c_str(), &findFileData);

    configFileNames.clear();

    if (hFind != INVALID_HANDLE_VALUE) {
        do {
            // Пропускаем папки "." и ".."
            if (wcscmp(findFileData.cFileName, L".") == 0 || wcscmp(findFileData.cFileName, L"..") == 0) {
                continue;
            }

            // Проверяем, что это файл, а не папка
            if (!(findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
                configFileNames.push_back(findFileData.cFileName);  // Добавляем имя файла в вектор
            }
        } while (FindNextFile(hFind, &findFileData) != 0);

        FindClose(hFind);
    }

    for (size_t i = 0; i < configFileNames.size(); i++)
    {
        std::wstring fileName = configFileNames[i];

        size_t dotPos = fileName.find_last_of(L".");
        if (dotPos != std::wstring::npos) {
            fileName = fileName.substr(0, dotPos);
        }
        AppendMenu(hTrayMenu, MF_STRING, BASE_ID + i, fileName.c_str());
    }
    AppendMenu(hTrayMenu, MF_SEPARATOR, 0, NULL);
    AppendMenu(hTrayMenu, MF_STRING, ID_TRAY_OPEN_FOLDER, L"Open Configs Folder");
    AppendMenu(hTrayMenu, MF_SEPARATOR, 0, NULL);
    AppendMenu(hTrayMenu, MF_STRING, ID_TRAY_SAVE_CURRENT, L"Save Current Config");
    AppendMenu(hTrayMenu, MF_SEPARATOR, 0, NULL);
    AppendMenu(hTrayMenu, MF_STRING, ID_TRAY_EXIT, L"Exit");
}

void ShowSaveDialog(HWND hWnd) {
    // Получаем путь к папке configs
    std::wstring configDir = GetExecutablePath() + L"\\configs";

    // Создаем папку, если она не существует
    CreateDirectory(configDir.c_str(), NULL);

    // Инициализируем структуру диалога
    OPENFILENAMEW ofn = { 0 };
    wchar_t fileName[MAX_PATH] = L"";

    ofn.lStructSize = sizeof(OPENFILENAMEW);
    ofn.hwndOwner = hWnd;
    ofn.lpstrFile = fileName;
    ofn.nMaxFile = MAX_PATH;
    ofn.lpstrInitialDir = configDir.c_str();
    ofn.lpstrFilter = L"Config Files (*.bin)\0*.bin\0All Files (*.*)\0*.*\0";
    ofn.nFilterIndex = 1;
    ofn.lpstrDefExt = L"bin";
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT;

    // Показываем диалог
    if (GetSaveFileNameW(&ofn)) {
        // Сохраняем данные в файл
        if (SerializeActiveDisplayConfig(fileName)) {
            MessageBoxW(hWnd, L"Ошибка сохранения файла!", L"Ошибка", MB_ICONERROR);
        }
    }
}

void OpenConfigsFolder() {
    // Получаем путь к папке configs
    std::wstring configPath = GetExecutablePath() + L"\\configs";

    // Проверяем существование папки
    DWORD attrib = GetFileAttributesW(configPath.c_str());
    if (attrib == INVALID_FILE_ATTRIBUTES || !(attrib & FILE_ATTRIBUTE_DIRECTORY)) {
        if (!CreateDirectoryW(configPath.c_str(), NULL)) {
            MessageBoxW(NULL, L"Папка configs не существует!", L"Ошибка", MB_ICONERROR);
            return;
        }
    }

    // Открываем папку в проводнике
    HINSTANCE result = ShellExecuteW(
        NULL,                   // Нет родительского окна
        L"open",               // Действие: открыть
        configPath.c_str(),    // Путь к папке
        NULL,                   // Параметры (не нужны)
        NULL,                   // Директория по умолчанию (не меняем)
        SW_SHOWNORMAL           // Режим отображения
    );

    // Обработка ошибок
    if ((int)result <= 32) {
        MessageBoxW(NULL, L"Не удалось открыть папку!", L"Ошибка", MB_ICONERROR);
    }
}

// Message handler
LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    std::wstring fpath;
    UINT_PTR commandId;
    switch (uMsg) {
    case WM_TRAYICON:
        if (lParam == WM_RBUTTONDOWN) {
            POINT p;
            GetCursorPos(&p);

            SetForegroundWindow(hWnd);
            TrackPopupMenu(hTrayMenu, TPM_BOTTOMALIGN | TPM_LEFTALIGN, p.x, p.y, 0, hWnd, NULL);
        }
        break;

    case WM_COMMAND:
        commandId = LOWORD(wParam);
        switch (commandId) {
        case ID_TRAY_SAVE_CURRENT:
            ShowSaveDialog(hWnd);  // Ваша функция для показа диалога
            UpdateTrayMenu();
            break;
        case ID_TRAY_OPEN_FOLDER:
            OpenConfigsFolder(); // Ваша функция для открытия папки
            break;
        case ID_TRAY_EXIT:
            Shell_NotifyIcon(NIM_DELETE, &nid); // Remove tray icon
            PostQuitMessage(0);
            break;
        default:
            if (commandId >= BASE_ID && commandId < BASE_ID + configFileNames.size()) {
                const size_t fileIndex = commandId - BASE_ID;
                fpath = GetExecutablePath() + L"/configs/" + configFileNames[fileIndex];

                DWORD attrib = GetFileAttributesW(fpath.c_str());
                if (attrib == INVALID_FILE_ATTRIBUTES || (attrib & FILE_ATTRIBUTE_DIRECTORY))
                {
                    MessageBoxW(hWnd,
                        L"Файл конфигурации не найден!",
                        L"Ошибка",
                        MB_ICONERROR | MB_OK
                    );
                    break;
                }

                DeserializeAndApplyDisplayConfig(fpath, DC_APPLY);
                /*MessageBoxW(hWnd,
                    L"Файл Конфигурации УСПЕШНО ПРИМЕНЕН",
                    L"Ошибка",
                    MB_ICONERROR | MB_OK
                );*/
            }
            break;
        }
        break;

    case WM_DESTROY:
        Shell_NotifyIcon(NIM_DELETE, &nid); // Remove tray icon on exit
        PostQuitMessage(0);
        break;
    }
    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow) {
    const wchar_t CLASS_NAME[] = L"QDCSWAP_Window";
    WNDCLASS wc = {};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;

    RegisterClass(&wc);

    HWND hWnd = CreateWindowEx(
        0,
        CLASS_NAME,
        L"QDCSWAP",
        WS_CAPTION,
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
        NULL,
        NULL,
        hInstance,
        NULL
    );

    if (hWnd == NULL) {
        return 0;
    }

    InitTrayIcon(hWnd);
    UpdateTrayMenu();

    // Message loop
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}