// Copyright (C) 2007-2013 Ivan Zhakov.

#ifdef HOOKDLL_EXPORTS
#define HOOKDLL_API __declspec(dllexport)
#else
#define HOOKDLL_API __declspec(dllimport)
#endif


extern HOOKDLL_API int nHookDll;

const LPCWSTR TFHookNotifyMsg = L"TouchFreezeNotifyMessage";

enum NotifyType
{
    TFNT_Blocked   = 1,
};

HOOKDLL_API int TFHookInstall     (HWND hwnd);
HOOKDLL_API int TFHookUninstall   ();

HOOKDLL_API void TFHookGetStat(int * pFreezeCount);
