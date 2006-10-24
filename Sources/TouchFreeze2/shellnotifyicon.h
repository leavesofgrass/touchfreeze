#pragma once

const UINT WM_SHELLICONNOTIFY = RegisterWindowMessage( _T("WM_SHELLICONNOTIFY") );

class CShellNotifyIcon
{
  public:
  
  CShellNotifyIcon();
 ~CShellNotifyIcon();
 
  void Create( HWND );
 
  void ChangeIcon( HICON   );
  void ChangeTips( LPCTSTR );
 
  private:
  
  NOTIFYICONDATA m_nid;
};
