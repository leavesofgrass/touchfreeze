#include "stdafx.h"
CComModule _Module;

#include "touchfreeze.h"

extern "C" 
void RunTouchFreezeHookW( HWND hwnd, HINSTANCE hinst, LPWSTR, int )
{
	_Module.Init( 0, hinst );
	
  CTouchFreezeWindow * tfWindow = new CTouchFreezeWindow();

  if( tfWindow->Init() == FALSE  )
    delete tfWindow;
  else  
  {
    pThis = tfWindow;
    
    MSG msg;
    while( GetMessage ( &msg, 0, 0, 0 ) )
    {
      TranslateMessage( &msg );
      DispatchMessage ( &msg );
    }
    
    delete tfWindow;
  }
  _Module.Term();
}
