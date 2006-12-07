#include "stdafx.h"

#include "HookDll.h"

static const LPCWSTR SharedDataMutexName = L"TouchFreezeGlobalDataMutex";

#pragma data_seg("shareddata")

HHOOK               g_hhookKeyboard = NULL;
HHOOK               g_hhookMouse    = NULL;

HWND                g_hWnd          = NULL;
DWORD               g_LastKeyTime   = 0;
DWORD               g_FreezeCount   = 0;
DWORD               g_FreezeTicks   = 700;
#pragma data_seg()

HANDLE              g_hMutex = NULL;
UINT                g_nNotifyMessage = ::RegisterWindowMessage(TFHookNotifyMsg);

#pragma comment(linker,"/SECTION:shareddata,SRW")

HINSTANCE g_hInstDLL;

BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
        case DLL_PROCESS_ATTACH:
            g_hInstDLL = (HINSTANCE) hModule;
            g_hMutex = ::CreateMutex(NULL, FALSE, SharedDataMutexName);
            break;
        
        case DLL_PROCESS_DETACH:
            ::CloseHandle(g_hMutex);
            break;

        case DLL_THREAD_ATTACH:
        case DLL_THREAD_DETACH:
            break;
    }
    return TRUE;
}

void SendNotify(NotifyType notifyType, LPARAM data)
{
    ::PostMessage(HWND_BROADCAST, g_nNotifyMessage, (WPARAM) notifyType, data);
}

static void LockGlobals()
{
    ::WaitForSingleObject(g_hMutex, INFINITE);
}

static void UnlockGlobals()
{
    ::ReleaseMutex(g_hMutex);
}

static bool IgnoredKey(DWORD vk)
{
    static DWORD ignoreKeys[] = {
        VK_CONTROL, VK_RCONTROL, VK_LCONTROL,
        VK_MENU,    VK_RMENU,    VK_LMENU,
        VK_SHIFT,   VK_RSHIFT,   VK_LSHIFT};
    for (int i = 0;i < sizeof(ignoreKeys)/sizeof(ignoreKeys[0]); i++)
    {
        if (ignoreKeys[i] == vk)
            return true;
    }
    
    return false;
}

__declspec(dllexport) LRESULT CALLBACK KeyboardHookProc( int code, 
                              WPARAM wParam, 
                              LPARAM lParam 
                              )
{
    KBDLLHOOKSTRUCT *pkbhs = (KBDLLHOOKSTRUCT *) lParam;

    if (code < 0) 
        return CallNextHookEx (g_hhookKeyboard, code, wParam, lParam);
    
    if ((pkbhs->flags & LLKHF_INJECTED) == 0 && !IgnoredKey(pkbhs->vkCode))
      g_LastKeyTime = GetTickCount();

    return CallNextHookEx (g_hhookKeyboard, code, wParam, lParam);
}

static bool IsBlockMouseMessage(UINT msg)
{
    static UINT ignoreMsgs[] = {
        WM_LBUTTONDOWN,   WM_MBUTTONDOWN,   WM_RBUTTONDOWN,
        WM_LBUTTONDBLCLK, WM_MBUTTONDBLCLK, WM_RBUTTONDBLCLK,
    };
   
    for (int i = 0;i < sizeof(ignoreMsgs)/sizeof(ignoreMsgs[0]); i++)
    {
        if (ignoreMsgs[i] == msg)
            return true;
    }
    
    return false;
}

__declspec(dllexport) LRESULT CALLBACK MouseHookProc( int code, 
                              WPARAM wParam, 
                              LPARAM lParam 
                              )
{
    MSLLHOOKSTRUCT  * pMouseHS = (MSLLHOOKSTRUCT *) lParam;
    
    if ((GetTickCount() - g_LastKeyTime) < g_FreezeTicks && IsBlockMouseMessage(wParam))
    {
        g_FreezeCount++;     
        
        if (g_hWnd)
            PostMessage(g_hWnd, g_nNotifyMessage, TFNT_Blocked, 0);
        
        return 1;
    }

    return CallNextHookEx (g_hhookMouse, code, wParam, lParam);
}

HOOKDLL_API int TFHookInstall (HWND hwnd)
{
    LockGlobals();
    TFHookUninstall();

    g_hWnd = hwnd;      
    g_hhookKeyboard = SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardHookProc, g_hInstDLL, 0);
    g_hhookMouse    = SetWindowsHookEx(WH_MOUSE_LL, MouseHookProc, g_hInstDLL, 0);
    UnlockGlobals();

    if (g_hhookKeyboard == NULL || g_hhookMouse == NULL)
    {
      MessageBox(NULL, L"Failed to setup hooks.", L"TouchFreeze", MB_OK);
    }

    return 0;
}

HOOKDLL_API int TFHookUninstall()
{
    LockGlobals();
    if (g_hhookKeyboard != NULL)
    {
        UnhookWindowsHookEx(g_hhookKeyboard);
        g_hhookKeyboard = NULL;
        UnhookWindowsHookEx(g_hhookMouse);
        g_hhookMouse = NULL;
    }
    UnlockGlobals();
    return 0;
}

HOOKDLL_API void TFHookGetStat(int * pFreezeCount)
{
    if(pFreezeCount)
        *pFreezeCount = g_FreezeCount;
}