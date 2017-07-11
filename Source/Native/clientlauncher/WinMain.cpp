//------------------------------------------------------------------------------
// File. WinMain.cpp
//------------------------------------------------------------------------------

#include "../FrameWork/StdAfx.h"
#include "resource.h"

#include "WinMain.h"

static int VertexProcessing = 0;
CD3DApp* g_pGameMain = NULL;

// 옵션대화상자 프로시저~
INT_PTR CALLBACK OptionDlgProc( HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam );

INT WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPreInst, LPSTR lpCmdLine, INT nShowCmd)
{
	// Option
	DialogBox( hInst, MAKEINTRESOURCE(IDD_DLG_OPTION),GetDesktopWindow(), (DLGPROC)OptionDlgProc );

	WNDCLASS wndClass = 
	{ 
		CS_DBLCLKS, 
		EntryMessageHandler, 
		0, 0, hInst, NULL, LoadCursor( NULL, IDC_ARROW ),(HBRUSH)GetStockObject( BLACK_BRUSH ),
		NULL, TEXT("hm..") 
	};

	RegisterClass( &wndClass );

	DWORD dwWindowStyle = WS_POPUP|WS_SYSMENU|CS_DBLCLKS;
	HWND hWnd = CreateWindowEx( 0, TEXT("hm.."), TEXT("hm.."), dwWindowStyle, 0, 0, 1024, 768, NULL, NULL, hInst, NULL );

	g_pGameMain = new CGameMain( hWnd, 1024, 768, string( lpCmdLine ), VertexProcessing );

	ShowWindow( hWnd, SW_SHOW );
	UpdateWindow( hWnd );

	g_pGameMain->Run();
	delete g_pGameMain;

	return 0;
}

LRESULT WINAPI EntryMessageHandler(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
	if( g_pGameMain != NULL )
	{
		return g_pGameMain->MessageHandler(hWnd, Message, wParam, lParam);
	}
	else
	{
		return DefWindowProc(hWnd, Message, wParam, lParam);
	}
}

INT_PTR CALLBACK OptionDlgProc( HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam )
{
    switch (msg)
    {
	case WM_INITDIALOG:
		SendDlgItemMessage(hDlg,IDC_COMBO1,CB_ADDSTRING,0,(LPARAM)"하드웨어 프로세싱");
		SendDlgItemMessage(hDlg,IDC_COMBO1,CB_ADDSTRING,0,(LPARAM)"소프트웨어 프로세싱");
		SendDlgItemMessage(hDlg,IDC_COMBO1,CB_SETCURSEL,0,0);
		return TRUE;
		
	case WM_COMMAND:
		switch( LOWORD(wParam) )
		{
		case IDOK:
			{
				int nSel = SendDlgItemMessage(hDlg,IDC_COMBO1,CB_GETCURSEL,0,0);

				switch(nSel) 
				{
				case 0: VertexProcessing = D3DCREATE_HARDWARE_VERTEXPROCESSING; break;
				case 1:	VertexProcessing = D3DCREATE_SOFTWARE_VERTEXPROCESSING; break;
				}
			}
			EndDialog( hDlg, TRUE );
			return TRUE;
		}
		break;
    }
	
    return FALSE;
}