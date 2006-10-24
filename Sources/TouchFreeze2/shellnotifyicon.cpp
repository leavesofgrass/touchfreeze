#include "stdafx.h"
#include "shellnotifyicon.h"

CShellNotifyIcon::CShellNotifyIcon()
{
  ZeroMemory( &m_nid, sizeof(m_nid) );
}

void CShellNotifyIcon::Create( HWND hwnd )
{
  m_nid.cbSize = sizeof(m_nid);
  m_nid.hWnd = hwnd;
  m_nid.uFlags = NIF_MESSAGE;
  m_nid.uCallbackMessage = WM_SHELLICONNOTIFY;
  m_nid.uID = 1;                
  
  ::Shell_NotifyIcon( NIM_ADD, &m_nid );
}

CShellNotifyIcon::~CShellNotifyIcon()
{
  if( m_nid.hWnd )
    ::Shell_NotifyIcon( NIM_DELETE, &m_nid );
}
 
void CShellNotifyIcon::ChangeIcon( HICON hicon )
{
  if( m_nid.hWnd )
  {
    m_nid.uFlags = NIF_ICON;
    m_nid.hIcon = hicon;
    ::Shell_NotifyIcon( NIM_MODIFY, &m_nid );
  }
}

void CShellNotifyIcon::ChangeTips( LPCTSTR tips )
{
  if( m_nid.hWnd )
  {
    m_nid.uFlags = NIF_TIP;
    _tcscpy( m_nid.szTip, tips );
    ::Shell_NotifyIcon( NIM_MODIFY, &m_nid );
  }
}
