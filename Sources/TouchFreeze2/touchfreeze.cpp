#include "stdafx.h"
#include "touchfreeze.h"

const LPCTSTR WINDOW_NAME = _T("{2e24b651-83e7-4d8b-ac0c-72fe28c950b4}");

const UINT_PTR START_TIMER_ID = 1000;

CTouchFreezeWindow * pThis = 0;

CTouchFreezeWindow::CTouchFreezeWindow()
{
  m_dwLastKeyTime = 0;
  m_dwFreezeTicks = 500;

  m_hHookKeyboard = 0;
  m_hHookMouse    = 0;
  
  m_hIconOFF = 0;
  m_hIconON  = 0;
  
  m_timerId = 0;
}

CTouchFreezeWindow::~CTouchFreezeWindow()
{
  if( m_hHookKeyboard )
    ::UnhookWindowsHookEx( m_hHookKeyboard );
  
  if( m_hHookMouse )
    ::UnhookWindowsHookEx( m_hHookMouse );
    
  if( IsWindow() )
    DestroyWindow();
    
  m_hWnd = 0;    
}

BOOL CTouchFreezeWindow::Init()
{
  if( ::FindWindow( 0, WINDOW_NAME ) )
    return FALSE;
    
  CWindowImpl<CTouchFreezeWindow>::Create( 0, 0, WINDOW_NAME, WS_POPUP );
  
  HINSTANCE hInstance = _Module.GetModuleInstance();
  
  m_hIconOFF = HICON(::LoadImage( hInstance, MAKEINTRESOURCE(IDR_ICON_OFF), IMAGE_ICON, 16, 16, 0));
  m_hIconON  = HICON(::LoadImage( hInstance, MAKEINTRESOURCE(IDR_ICON_ON),  IMAGE_ICON, 16, 16, 0));
  
  m_shni.Create( *this );
  m_shni.ChangeIcon( m_hIconOFF );
  m_shni.ChangeTips( _T("TouchFreeze2") );
  
  m_hHookKeyboard = ::SetWindowsHookEx( WH_KEYBOARD_LL, KeyboardHookProc, hInstance, 0 );
  m_hHookMouse    = ::SetWindowsHookEx( WH_MOUSE_LL,    MouseHookProc,    hInstance, 0 );
  
  m_àboutDlg.Create( 0 );
  m_àboutDlg.ShowWindow( SW_SHOW );
  
  SetTimer( START_TIMER_ID, 2000 );
  
  m_settingsDlg.Create( 0 );              
  
  return TRUE;
}

LRESULT CALLBACK CTouchFreezeWindow::KeyboardHookProc( int code, WPARAM wp, LPARAM lp )
{
  return ((CTouchFreezeWindow*)pThis)->OnKeyboardHookProc( code, wp, lp );
}

LRESULT CALLBACK CTouchFreezeWindow::MouseHookProc( int code, WPARAM wp, LPARAM lp )
{
  return ((CTouchFreezeWindow*)pThis)->OnMouseHookProc( code, wp, lp );
} 
                            
LRESULT CTouchFreezeWindow::OnKeyboardHookProc( int code, WPARAM wp, LPARAM lp )
{
  KBDLLHOOKSTRUCT * pKbHS = (KBDLLHOOKSTRUCT*)lp;

  BOOL flagIgnoreKey = FALSE;
  
  static DWORD ignoreKeys[] = 
  {
    VK_CONTROL, VK_RCONTROL, VK_LCONTROL,
    VK_MENU,    VK_RMENU,    VK_LMENU,
    VK_SHIFT,   VK_RSHIFT,   VK_LSHIFT
  };
  
  for( int i = 0; i < sizeof(ignoreKeys)/sizeof(ignoreKeys[0]); i++ )
  {
    if( ignoreKeys[i] == pKbHS->vkCode )
    {
      flagIgnoreKey = TRUE;
      break;
    }  
  }

  if ( code >= 0 && 
       (pKbHS->flags & LLKHF_INJECTED) == 0 &&
       !flagIgnoreKey )
    m_dwLastKeyTime = GetTickCount();

  return ::CallNextHookEx( m_hHookKeyboard, code, wp, lp);
}

LRESULT CTouchFreezeWindow::OnMouseHookProc( int code, WPARAM wp, LPARAM lp )
{
  MSLLHOOKSTRUCT * pMsHS = (MSLLHOOKSTRUCT*)lp;

  BOOL flagIgnoreMsg = FALSE;
                                                                      
  static DWORD ignoreMsgs[] = 
  {
    WM_LBUTTONDOWN,   WM_MBUTTONDOWN,   WM_RBUTTONDOWN,
    WM_LBUTTONDBLCLK, WM_MBUTTONDBLCLK, WM_RBUTTONDBLCLK
  };
  
  for( int i = 0; i < sizeof(ignoreMsgs)/sizeof(ignoreMsgs[0]); i++ )
  {
    if( ignoreMsgs[i] == wp )
    {
      flagIgnoreMsg = TRUE;
      break;
    }  
  }

  DWORD dt = GetTickCount() - m_dwLastKeyTime;
  
  if ( flagIgnoreMsg && dt < m_dwFreezeTicks )
  {     
    if ( m_timerId == 0 )
      PostMessage( WM_TOUCHFREEZEON, 0, 0 );
    
    return 1;
  }  

  return ::CallNextHookEx( m_hHookMouse, code, wp, lp);
} 

LRESULT CTouchFreezeWindow::OnWmShellNotify( UINT, WPARAM, LPARAM lp, BOOL& )
{
  if( (UINT)lp == WM_LBUTTONUP || (UINT)lp == WM_RBUTTONUP )
    TrackPopupMenu();

  return 1;
}

LRESULT CTouchFreezeWindow::OnWmTouchFreezeOn( UINT, WPARAM, LPARAM lp, BOOL& )
{
  if( m_timerId ) 
    KillTimer( m_timerId );
  
  m_timerId = SetTimer( 1, m_dwFreezeTicks );

  m_shni.ChangeIcon( m_hIconON );
  return 1;
}

LRESULT CTouchFreezeWindow::OnWmTimer( UINT, WPARAM wp, LPARAM, BOOL& )
{
  if( wp == START_TIMER_ID )
  {
    m_àboutDlg.ShowWindow( SW_HIDE );
    KillTimer( START_TIMER_ID );
    return 1;
  }
  
  KillTimer( m_timerId );
  m_timerId = 0; 

  m_shni.ChangeIcon( m_hIconOFF );
  return 1;
}

void CTouchFreezeWindow::TrackPopupMenu()
{
  HINSTANCE hInstance = _Module.GetModuleInstance();
  
  HMENU hMenu      = ::LoadMenu  ( hInstance, MAKEINTRESOURCE(IDR_MENU) );
  HMENU hMenuPopup = ::GetSubMenu( hMenu, 0);
  
  ::SetMenuDefaultItem( hMenuPopup, ID_SETTINGS, FALSE );
  
  POINT pt; ::GetCursorPos( &pt );
                                                
  ::SetForegroundWindow( *this );
  
  int res = ::TrackPopupMenu( hMenuPopup, TPM_LEFTALIGN | TPM_RIGHTBUTTON,
                    pt.x, pt.y, 0, *this, 0 );
  
  PostMessage( WM_NULL, 0, 0 );

  DestroyMenu( hMenu );
}

LRESULT CTouchFreezeWindow::OnCmdExit( WORD, WORD, HWND, BOOL& )
{
  PostQuitMessage( 0 );                                         
  return 1;    
}

LRESULT CTouchFreezeWindow::OnCmdAbout( WORD, WORD, HWND, BOOL& )
{
  m_àboutDlg.ShowWindow( SW_SHOW );
  m_àboutDlg.SetActiveWindow();
  m_àboutDlg.BringWindowToTop();
    
  return 1;
}

LRESULT CTouchFreezeWindow::OnCmdSettings( WORD, WORD, HWND, BOOL& )
{
  m_settingsDlg.ShowWindow( SW_SHOW );
  m_settingsDlg.SetActiveWindow();
  m_settingsDlg.BringWindowToTop();
    
  return 1;
}