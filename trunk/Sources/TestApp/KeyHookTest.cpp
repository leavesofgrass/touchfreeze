#include "stdafx.h"

#include "..\HookDll\HookDll.h"
#include "AboutDlg.h"
#include "resource.h"
#include "Constants.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

const UINT     wm_KBHookNotify = RegisterWindowMessage( TFHookNotifyMsg );
const UINT     wm_ShellNotify = WM_APP + 1;
NOTIFYICONDATA m_NotifyIcon;
HICON          g_hIcon;

HINSTANCE      g_hInst = NULL;

static LONG RegSetStringValue(HKEY hKey, LPCTSTR valueName, LPCTSTR value)
{
    return ::RegSetValueEx(hKey, valueName, 0, REG_SZ,
        (const BYTE *) value, _tcslen(value)*sizeof(TCHAR)); 
}

static void OpenURL(HWND hwnd, LPCTSTR url)
{
    ShellExecute(hwnd, NULL, url, NULL, NULL, SW_SHOWNORMAL);
}

void ContactOrDonate(HWND hwnd, int type)
{
    switch(type)
    {
    case 1: OpenURL(hwnd, DONATE_URL); break;
    case 2: OpenURL(hwnd, WEBSITE_URL);  break;
    }
}

static void SetAutorun(BOOL autoRun)
{
    HKEY  regKey;
    TCHAR moduleFileName[_MAX_PATH];
    DWORD rv;

    rv = GetModuleFileName(g_hInst, moduleFileName, _MAX_PATH);
    if (rv != ERROR_SUCCESS)
    {
        return ;
    }

    // Create provider key
    rv = RegCreateKey(HKEY_CURRENT_USER, AUTORUN_KEY, &regKey);
    if (rv != ERROR_SUCCESS)
    {
        return ;
    }

    if (autoRun)
        RegSetStringValue(regKey, TOUCHFREEZE_KEY, moduleFileName);
    else
        RegDeleteValue(regKey, TOUCHFREEZE_KEY);

    RegCloseKey(regKey);
}

static BOOL IsAutorun()
{
    HKEY  regKey;
    TCHAR moduleFileName[_MAX_PATH];
    TCHAR regFileName   [_MAX_PATH];
    DWORD rv;

    rv = GetModuleFileName(g_hInst, moduleFileName, _MAX_PATH);
    if (rv != ERROR_SUCCESS)
    {
        return FALSE;
    }

    // Create provider key
    if (RegOpenKeyEx(HKEY_CURRENT_USER, AUTORUN_KEY, 0, KEY_ALL_ACCESS, &regKey) 
        != ERROR_SUCCESS)
    {
        return FALSE;
    }

    DWORD size = sizeof(regFileName);
    
    if (RegQueryValueEx(regKey, TOUCHFREEZE_KEY, NULL, NULL, (LPBYTE) regFileName, &size) 
        != ERROR_SUCCESS)
    {
        regFileName[0] = 0;
    }
    
    RegCloseKey(regKey);
    return _tcsicmp(regFileName, moduleFileName) == 0;
}

static void ShowContextMenu(HWND hwnd)
{
    HMENU hMenu = LoadMenu(g_hInst, MAKEINTRESOURCE(IDR_MENU));
    HMENU hMenuPopup = GetSubMenu(hMenu, 0);
    
    if (IsAutorun())
        DeleteMenu(hMenu, ID_AUTOSTART_ON, MF_BYCOMMAND);
    else
        DeleteMenu(hMenu, ID_AUTOSTART_OFF, MF_BYCOMMAND);

    POINT pt;
    GetCursorPos(&pt);
 
    SetForegroundWindow(hwnd);

    TrackPopupMenu(hMenuPopup, TPM_LEFTBUTTON|TPM_LEFTALIGN,
        pt.x, pt.y, 0, hwnd, NULL);

    PostMessage(hwnd, WM_NULL, 0, 0);

    DestroyMenu(hMenu);
}

LRESULT CALLBACK MainWindowProc(
   HWND hWnd,
   UINT uMsg,
   WPARAM wParam,
   LPARAM lParam 
)
{
    switch(uMsg)
    {
    case WM_CREATE:
        ZeroMemory(&m_NotifyIcon, sizeof(m_NotifyIcon));
        m_NotifyIcon.cbSize = sizeof(m_NotifyIcon);
        m_NotifyIcon.hIcon = g_hIcon;
        m_NotifyIcon.hWnd = hWnd;
        m_NotifyIcon.uFlags = NIF_ICON|NIF_MESSAGE|NIF_TIP;
        m_NotifyIcon.uCallbackMessage = wm_ShellNotify;
        m_NotifyIcon.uID = 1;                
        _tcscpy_s(m_NotifyIcon.szTip, sizeof(m_NotifyIcon.szTip),
                  _T("TouchFreeze (Automatic mode)"));
        Shell_NotifyIcon(NIM_ADD, &m_NotifyIcon);
        return 0;

    case WM_DESTROY:
        Shell_NotifyIcon(NIM_DELETE, &m_NotifyIcon);
        return 0;

    case WM_COMMAND:
        switch(LOWORD(wParam))
        {
        case ID_ABOUT:
            ShowAboutDlg(g_hInst, hWnd);
            break;
        
        case ID_EXIT:
            PostQuitMessage(1);
            break;
        
        case ID_AUTOSTART_ON:
            SetAutorun(TRUE);
            break;
        
        case ID_AUTOSTART_OFF:
            SetAutorun(FALSE);
            break;

        case ID_DONATE:
            ContactOrDonate(hWnd, 1);
            break;
        }
        return 0;

    case WM_CLOSE:
        PostQuitMessage(1);
        return 0;
    }

    if (uMsg == wm_KBHookNotify)
    {
        return 0;
    }
    else if (uMsg == wm_ShellNotify)
    {
        switch(lParam)
        {
        case WM_LBUTTONUP:
        case WM_RBUTTONUP:
            ShowContextMenu(hWnd);
            break;
        }
        return 0;
    }

    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

int WINAPI _tWinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPTSTR lpCmdLine, int nCmdShow)
{
    g_hInst = hInst;

    HWND hPrevHWND = FindWindow(WINDOW_CLASS_NAME, WINDOW_NAME);

    if (hPrevHWND)
        PostMessage(hPrevHWND, WM_CLOSE, 0, 0);

    if(_tcsstr(lpCmdLine, _T("unload")) != 0)
    {
        return 0;
    }


    WNDCLASSEX wndClass;
    ZeroMemory(&wndClass, sizeof(wndClass));
    wndClass.cbSize        = sizeof(wndClass);
    wndClass.hInstance     = hInst;
    wndClass.lpfnWndProc   = MainWindowProc;
    wndClass.lpszClassName = WINDOW_CLASS_NAME;
    if (!RegisterClassEx(&wndClass))
        return -1;

    g_hIcon = (HICON) ::LoadImage(hInst, MAKEINTRESOURCE(IDR_MAINFRAME),
        IMAGE_ICON, 16, 16, 0);

    HWND hwnd = CreateWindow(WINDOW_CLASS_NAME, WINDOW_NAME, 
        WS_OVERLAPPED, 0,0,0,0, HWND_MESSAGE, NULL, hInst, NULL);

    if (!hwnd)
        return -2;

    TFHookInstall(hwnd);

    MSG msg;
    while( GetMessage( &msg, NULL, 0, 0 ))
    { 
        TranslateMessage(&msg); 
        DispatchMessage(&msg); 
    }
    
    TFHookUninstall();
    
    DestroyWindow(hwnd);
    UnregisterClass(WINDOW_CLASS_NAME, hInst);
    return 0;
}
