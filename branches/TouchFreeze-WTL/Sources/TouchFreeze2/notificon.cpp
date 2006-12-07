#include "stdafx.h"
#include "notificon.h"

CNotifIcon::CNotifIcon( HWND hwnd, HICON hicon )
{
  ZeroMemory( &m_nid, sizeof(m_nid) );
  m_nid.cbSize = sizeof(m_nid);
  m_nid.hIcon = hicon;
  m_nid.hWnd = hwnd;
  m_nid.uFlags = NIF_ICON|NIF_MESSAGE;
  m_nid.uCallbackMessage = WM_SHELLNOTIFY;
  m_nid.uID = 1;                
  ::Shell_NotifyIcon( NIM_ADD, &m_nid );
}

CNotifIcon::~CNotifIcon()
{
}
 
void CNotifIcon::ChangeIcon( HICON hicon )
{
  m_nid.uFlags = NIF_ICON;
  m_nid.hIcon = hicon;
  Shell_NotifyIcon( NIM_MODIFY, &m_nid);
}

void CNotifIcon::ChangeTips( LPCTSTR tips )
{
  m_nid.uFlags = NIF_TIP;
  _tcscpy( m_nid.szTip, tips );
  Shell_NotifyIcon( NIM_MODIFY, &m_nid);
}
