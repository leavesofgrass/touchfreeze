#pragma once

#include "resource.h"
#include "shellnotifyicon.h"
#include "aboutdlg.h"
#include "settingsdlg.h"

const UINT WM_TOUCHFREEZEON  = RegisterWindowMessage( _T("WM_TOUCHFREEZEON" ) );

class CTouchFreezeWindow;
extern CTouchFreezeWindow * pThis;

class CTouchFreezeWindow :
  public CWindowImpl<CTouchFreezeWindow>
{
  public:
  
  CTouchFreezeWindow();
 ~CTouchFreezeWindow();
  
  BOOL Init();
  
  protected:
  
  BEGIN_MSG_MAP(CTouchFreezeWindow)
    MESSAGE_HANDLER(WM_SHELLICONNOTIFY,OnWmShellNotify)
    COMMAND_ID_HANDLER(ID_EXIT,OnCmdExit)
    COMMAND_ID_HANDLER(ID_ABOUT,OnCmdAbout)
    COMMAND_ID_HANDLER(ID_SETTINGS,OnCmdSettings)
    MESSAGE_HANDLER(WM_TOUCHFREEZEON,OnWmTouchFreezeOn)
    MESSAGE_HANDLER(WM_TIMER,OnWmTimer)
  END_MSG_MAP()
  
  LRESULT OnWmShellNotify   ( UINT, WPARAM, LPARAM, BOOL& );
  LRESULT OnCmdExit         ( WORD, WORD,   HWND,   BOOL& );
  LRESULT OnCmdAbout        ( WORD, WORD,   HWND,   BOOL& );
  LRESULT OnCmdSettings     ( WORD, WORD,   HWND,   BOOL& );
  LRESULT OnWmTouchFreezeOn ( UINT, WPARAM, LPARAM, BOOL& );
  LRESULT OnWmTouchFreezeOff( UINT, WPARAM, LPARAM, BOOL& );
  LRESULT OnWmTimer         ( UINT, WPARAM, LPARAM, BOOL& );
  
  private:
  
  HHOOK m_hHookKeyboard;
  HHOOK m_hHookMouse;
  
  static LRESULT CALLBACK KeyboardHookProc( int code, WPARAM wp, LPARAM lp );
  static LRESULT CALLBACK MouseHookProc   ( int code, WPARAM wp, LPARAM lp ); 
                              
  LRESULT OnKeyboardHookProc( int code, WPARAM wp, LPARAM lp );
  LRESULT OnMouseHookProc   ( int code, WPARAM wp, LPARAM lp ); 
  
  DWORD m_dwLastKeyTime;
  DWORD m_dwFreezeTicks;
  
  void TrackPopupMenu();
  
  CShellNotifyIcon m_shni;
  
  HICON m_hIconOFF;
  HICON m_hIconON;
  
  CAboutDialog    m_àboutDlg;
  CSettingsDialog m_settingsDlg;
  
  UINT_PTR m_timerId;
};  