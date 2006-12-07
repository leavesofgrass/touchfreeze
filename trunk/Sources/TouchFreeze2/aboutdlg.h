#pragma once

#include "resource.h"

class CAboutDialog :
  public CDialogImpl<CAboutDialog>
{
  public:
  
  enum { IDD = IDD_DIALOG_ABOUT };
  
 ~CAboutDialog()
  {
    if( IsWindow() )
      DestroyWindow();
    
    m_hWnd = 0;
  }      
  
  BEGIN_MSG_MAP(CAboutDialog)
    COMMAND_ID_HANDLER(IDCANCEL,OnCmdCancel)
  END_MSG_MAP()
  
  LRESULT OnCmdCancel( WORD, WORD, HWND, BOOL& )
  {
    ShowWindow( SW_HIDE );
    return 1;
  }
};