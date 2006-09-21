// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the HOOKDLL_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// HOOKDLL_API functions as being imported from a DLL, wheras this DLL sees symbols
// defined with this macro as being exported.

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
