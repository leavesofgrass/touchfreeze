#pragma once

#include "resource.h"

class CSettingsDialog :
  public CDialogImpl<CSettingsDialog>
{
  public:
  
  enum { IDD = IDD_DIALOG_SETTINGS };
  
  CSettingsDialog();
 ~CSettingsDialog();
  
  BEGIN_MSG_MAP(CSettingsDialog)
    MESSAGE_HANDLER(WM_INITDIALOG,OnWmInitDialog)
    COMMAND_ID_HANDLER(IDOK,OnCmdOk)
    COMMAND_ID_HANDLER(IDCANCEL,OnCmdCancel)                
    COMMAND_HANDLER(IDC_AUTOLOAD_CHECK,BN_CLICKED,OnBnClicked)
  END_MSG_MAP()
  
  LRESULT OnWmInitDialog( UINT, WPARAM, LPARAM, BOOL& );
  LRESULT OnCmdOk       ( WORD, WORD, HWND, BOOL& );
  LRESULT OnCmdCancel   ( WORD, WORD, HWND, BOOL& );
  LRESULT OnBnClicked   ( WORD code, WORD id, HWND, BOOL& );
  
  private:
  
  BOOL m_bLoadAtStartup;
};