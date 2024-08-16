#include "framework.h"
#include "HWS_DisplayPrintCheck.h"
#include "resource.h"
#include <windows.h>
#include <shellapi.h>
#include <commctrl.h>
#include <strsafe.h>
#include <string>
#include <fstream>
#include <thread>
#include <chrono>
#include <functional>

// Ensure necessary headers are included
#include <iostream> // For std::getline
#include <cstdlib>  // For _dupenv_s

std::string pathAppData(std::string fileName)
{
    std::string returnVal("");
    char* buf = nullptr;
    size_t sz = 0;
    if (_dupenv_s(&buf, &sz, "APPDATA") == 0 && buf != nullptr)
    {
        returnVal += buf;
        returnVal += fileName;
    }
    free(buf);
    return returnVal;
}


bool shouldWeCloneScreen()
{
    TCHAR  compName[INFO_BUFFER_SIZE];
    DWORD  bufCharCount = INFO_BUFFER_SIZE;
    GetComputerName(compName, &bufCharCount);

    std::string szPCName = TCharToString(compName);

    if (fileExists("c:\\yesclone.txt") || fileExists(pathAppData("\\yesclone.txt")))
        return true;

    if (fileExists("c:\\dontclone.txt") || fileExists(pathAppData("\\dontclone.txt")))
    {
        if (GetDisplayConf() != DISPLAYCONFIG_TOPOLOGY_EXTEND)
            SetDisplayConfig(0, NULL, 0, NULL, SDC_TOPOLOGY_EXTEND | SDC_APPLY);

        return false;
    }

    //Check PC Name

    if (szPCName.find("TPC") != std::string::npos) {
        return true;
    }
    if (szPCName.find("Tpc") != std::string::npos) {
        return true;
    }
    if (szPCName.find("SCREEN") != std::string::npos) {
        return true;
    }
    if (szPCName.find("tpc") != std::string::npos) {
        return true;
    }
    if (szPCName.find("screen") != std::string::npos) {
        return true;
    }
    return false;
}


void timer_start(std::function<void(void)> func, unsigned int interval)
{
    std::thread([func, interval]() {
        while (true)
        {
            func();
            std::this_thread::sleep_for(std::chrono::milliseconds(interval));
        }
        }).detach();
}


void checkDisplays()
{
    if (GetDisplayConf() != DISPLAYCONFIG_TOPOLOGY_CLONE)
        SetDisplayConfig(0, NULL, 0, NULL, SDC_TOPOLOGY_CLONE | SDC_APPLY);

}

void checkPrinter() {
    if (fileExists(pathAppData("\\disableprintchanging.txt")))
        return;

    std::string wantedPrinter;
    std::ifstream file(pathAppData("\\defaultprinter.txt"));
    if (file.is_open()) {
         std::getline(file, wantedPrinter);
        file.close();
        setDefaultPrinter(wantedPrinter);
        return;
    }
    setDefaultPrinter("FOLLOW");
    setDefaultPrinter("Follow");
    setDefaultPrinter("follow");
}


void ConfigurePrinterOptions()
{
    if (DisplayMessageBox_YesNo("Do you want this program to keep changing your default printer to Follow Me?"))
    {
        remove(pathAppData("\\defaultprinter.txt").c_str());
        remove(pathAppData("\\disableprintchanging.txt").c_str());
        return;
    }
    else
    {
        createBlankFile(pathAppData("\\disableprintchanging.txt"));
    }
    if (DisplayMessageBox_YesNo("Do you want a DEFAULT printer which is __NOT__ Follow me?"))
    {
        DisplayMessageBox_OnlyMessage("Create a text file called %APPDATA%\\defaultprinter.txt with partial name of printer you want. For example ROOM13 - Ask IT for assistance. Then login and out.");
        return;
    }

}
void ConfigureScreenOptions()
{
    if (DisplayMessageBox_YesNo("Do you want this program to keep cloning/duplicating your displays?\nYes = Clone Displays (Good for Teacher PCs)\nNo = Extended Monitors (Good for Office PCs)"))
    {
        if (DisplayMessageBox_YesNo("Do you want to FORCE CLONE SCREENS even on an office PC?\n NO = Recommended!"))
        {
            createBlankFile(pathAppData("\\yesclone.txt"));
        }
        remove(pathAppData("\\dontclone.txt").c_str());
    }
    else
    {
        remove(pathAppData("\\yesclone.txt").c_str());
        createBlankFile(pathAppData("\\dontclone.txt"));
        SetDisplayConfig(0, NULL, 0, NULL, SDC_TOPOLOGY_EXTEND | SDC_APPLY);
    }
    if (DisplayMessageBox_YesNo("Do you want this program to set your DISPLAY SCALING to 100%?\n YES = Recommended!"))
    {
        remove(pathAppData("\\dontdpi.txt").c_str());
    }
    else
    {
        createBlankFile(pathAppData("\\dontdpi.txt"));
    }

    DisplayMessageBox_OnlyMessage("Log in and out for changes to take affect.");
}

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Place code here.

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_HWSDISPLAYCHECK, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance(hInstance, nCmdShow))
    {
        return FALSE;
    }


    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_HWSDISPLAYCHECK));





    if (shouldWeCloneScreen())
    {
        if (!fileExists(pathAppData("\\dontdpi.txt")))
            SetDpiScaling(100);

        timer_start(checkDisplays, 1000);
    }



     timer_start(checkPrinter, 30000);



    MSG msg;

    // Main message loop:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}

UINT const WMAPP_NOTIFYCALLBACK = WM_APP + 1;
UINT const WMAPP_HIDEFLYOUT = WM_APP + 2;
UINT_PTR const HIDEFLYOUT_TIMER_ID = 1;
HINSTANCE g_hInst = NULL;
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_HWSDISPLAYCHECK));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_HWSDISPLAYCHECK);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));
    g_hInst = hInstance;
    return RegisterClassExW(&wcex);
}


BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Store instance handle in our global variable

    // Create a ghost window but do not display
   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      0, 0, 0, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   //ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}
BOOL RestoreTooltip()
{
    // After the balloon is dismissed, restore the tooltip.
    NOTIFYICONDATA nid = { sizeof(nid) };
    nid.uFlags = NIF_SHOWTIP | NIF_GUID;

    return Shell_NotifyIcon(NIM_MODIFY, &nid);
}

BOOL AddNotificationIcon(HWND hwnd) {
    NOTIFYICONDATA nid = { sizeof(nid) };
    nid.hWnd = hwnd;
    nid.uID = 1001; // An identifier for the icon
    nid.uFlags = NIF_ICON | NIF_TIP | NIF_MESSAGE;
    nid.uCallbackMessage = WMAPP_NOTIFYCALLBACK;
    nid.hIcon = LoadIcon(g_hInst, MAKEINTRESOURCE(IDI_HWSDISPLAYCHECK));
    LoadString(g_hInst, IDS_TOOLTIP, nid.szTip, ARRAYSIZE(nid.szTip));

    if (!Shell_NotifyIcon(NIM_ADD, &nid)) {
        MessageBox(hwnd, L"Failed to add notification icon!", L"Error", MB_OK | MB_ICONERROR);
        return FALSE;
    }

    // Set the version for the icon, NOTIFYICON_VERSION_4 is preferred
    nid.uVersion = NOTIFYICON_VERSION_4;
    if (!Shell_NotifyIcon(NIM_SETVERSION, &nid)) {
        MessageBox(hwnd, L"Failed to set notification icon version!", L"Error", MB_OK | MB_ICONERROR);
        return FALSE;
    }

    return TRUE;
}



void ShowContextMenu(HWND hwnd, POINT pt)
{
    HMENU hMenu = LoadMenu(g_hInst, MAKEINTRESOURCE(IDC_CONTEXTMENU));
    if (hMenu)
    {
        HMENU hSubMenu = GetSubMenu(hMenu, 0);
        if (hSubMenu)
        {
            // our window must be foreground before calling TrackPopupMenu or the menu will not disappear when the user clicks away
            SetForegroundWindow(hwnd);

            // respect menu drop alignment
            UINT uFlags = TPM_RIGHTBUTTON;
            if (GetSystemMetrics(SM_MENUDROPALIGNMENT) != 0)
            {
                uFlags |= TPM_RIGHTALIGN;
            }
            else
            {
                uFlags |= TPM_LEFTALIGN;
            }

            TrackPopupMenuEx(hSubMenu, uFlags, pt.x, pt.y, hwnd, NULL);
        }
        DestroyMenu(hMenu);
    }
}


LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
    case WM_CREATE:
        AddNotificationIcon(hWnd);
        break; // Added break statement here
    case WMAPP_NOTIFYCALLBACK:
        switch (LOWORD(lParam)) {
        case NIN_SELECT:
            POINT const pt = { LOWORD(wParam), HIWORD(wParam) };
            ShowContextMenu(hWnd, pt);
            break; // Added break statement here
        }
        break; // Added break statement here
    case WM_COMMAND:
        int wmId;
        wmId = LOWORD(wParam);
        switch (wmId) {
        case IDM_EXIT:
            DestroyWindow(hWnd);
            break; // Added break statement here
        case IDM_DEFPRINTER:
            ConfigurePrinterOptions();
            break; // Added break statement here
        case IDM_DEFSCREEN:
            ConfigureScreenOptions();
            break; // Added break statement here
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
        break; // Added break statement here
    case WM_PAINT:
        PAINTSTRUCT ps;
        HDC hdc;
        hdc = BeginPaint(hWnd, &ps);
        EndPaint(hWnd, &ps);
        break; // Added break statement here
    case WM_DESTROY:
        PostQuitMessage(0);
        break; // Added break statement here
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}