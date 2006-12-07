#include "stdafx.h"
#include "settingsdlg.h"

CSettingsDialog::CSettingsDialog()
{
}

CSettingsDialog::~CSettingsDialog()
{
  if( IsWindow() )
    DestroyWindow();
  
  m_hWnd = 0;
}      

LRESULT CSettingsDialog::OnWmInitDialog( UINT, WPARAM, LPARAM, BOOL& )
{
  return 0;
}

LRESULT CSettingsDialog::OnBnClicked( WORD, WORD, HWND, BOOL& )
{
  ::EnableWindow( GetDlgItem( IDOK ), TRUE );
  return 0;
}

LRESULT CSettingsDialog::OnCmdOk( WORD, WORD, HWND, BOOL& )
{
  ::EnableWindow( GetDlgItem( IDOK ), FALSE );
  return 0;
}

LRESULT CSettingsDialog::OnCmdCancel( WORD, WORD, HWND, BOOL& )
{
  ShowWindow( SW_HIDE );
  return 0;
}
