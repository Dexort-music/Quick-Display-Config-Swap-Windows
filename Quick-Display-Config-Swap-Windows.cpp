#include <windows.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <cstdio>
#include "Utils.h"
#include "DisplayConfig.h"
#include <shellapi.h>

#define WM_TRAYICON (WM_USER + 1)
#define ID_TRAY_EXIT 1002

NOTIFYICONDATA nid;
HMENU hTrayMenu;

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

void InitTrayMenu() {
    hTrayMenu = CreatePopupMenu();
    AppendMenu(hTrayMenu, MF_STRING, 1, L"Setup1");
    AppendMenu(hTrayMenu, MF_STRING, 2, L"Setup2");
    AppendMenu(hTrayMenu, MF_STRING, 3, L"Setup3");
    AppendMenu(hTrayMenu, MF_SEPARATOR, 0, NULL);
    AppendMenu(hTrayMenu, MF_STRING, ID_TRAY_EXIT, L"Exit");
}

// Message handler
LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    std::string fpath;
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
        switch (LOWORD(wParam)) {
        case 1:
            fpath = GetExecutablePath() + "/configs/1.bin";
            DeserializeAndApplyDisplayConfig(fpath, DC_APPLY);
            //MessageBox(NULL, L"Hello from the tray!", L"Tray Message", MB_OK | MB_ICONINFORMATION);
            break;
        case 2:
            fpath = GetExecutablePath() + "/configs/2.bin";
            DeserializeAndApplyDisplayConfig(fpath, DC_APPLY);
            //MessageBox(NULL, L"Hello from the tray!", L"Tray Message", MB_OK | MB_ICONINFORMATION);
            break;
        case 3:
            fpath = GetExecutablePath() + "/configs/3.bin";
            DeserializeAndApplyDisplayConfig(fpath, DC_APPLY);
            //MessageBox(NULL, L"Hello from the tray!", L"Tray Message", MB_OK | MB_ICONINFORMATION);
            break;
        case ID_TRAY_EXIT:
            Shell_NotifyIcon(NIM_DELETE, &nid); // Remove tray icon
            PostQuitMessage(0);
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
    InitTrayMenu();

    // Message loop
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}