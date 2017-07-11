//------------------------------------------------------------------------------
// File. D3DApp.cpp
//------------------------------------------------------------------------------

#include "StdAfx.h"
#include "D3DApp.h"

CD3DApp::CD3DApp( HWND hWnd, DWORD dwWidth, DWORD dwHegiht, int vertexProcessing )
: m_hWndMain( hWnd )
, m_dwScreenWidth( dwWidth )
, m_dwScreenHeight( dwHegiht )
, m_vertexProcessing( vertexProcessing )
, m_bStart( false )
{
	if( FAILED(this->InitD3DDevice()) )
	{
		DestroyWindow( m_hWndMain );
	}

//#ifdef _DEBUG
	m_pFPSFont = NULL;
	if( FAILED( D3DXCreateFont( m_pD3DDevice, 16, 0, FW_BOLD, 0, FALSE, DEFAULT_CHARSET, 
		OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, 
		TEXT("Italic"), &m_pFPSFont ) ) )
	{
	}
//#endif
}

CD3DApp::~CD3DApp()
{
}

//-----------------------------------------------------------------
// Window 및 d3d 생성
//-----------------------------------------------------------------
HRESULT CD3DApp::Create( HWND hWndParent )
{
	return S_OK;
}

VOID CD3DApp::UpdateFrameRate()
{
	static DWORD dwFrame = 0;
	static DWORD dwOldTime = 0;

	++dwFrame;
	DWORD dwTime = GetTickCount() - dwOldTime;
	if(dwTime > 1000)
	{
		m_fFPS = dwFrame * 1000.0F / dwTime;
		dwOldTime = GetTickCount();
		dwFrame = 0;
	}
}

bool CD3DApp::Stop()
{
	m_bStart = false;
	return true;
}

bool CD3DApp::Run()
{
	m_bStart = true;

    MSG  msg;
	ZeroMemory( &msg, sizeof(msg) );

	// 파일 및 각종 객체 로딩
	this->LoadScene();

	while( WM_QUIT != msg.message && m_bStart )
    {
		if( PeekMessage(&msg,NULL,0,0,PM_REMOVE) )
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			m_pD3DDevice->Clear( 0L, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(131,181,214), 1.0f, 0L );

			this->FrameMove();	// 데이터 처리

			if( SUCCEEDED( m_pD3DDevice->BeginScene() ) )
			{
				this->Render();

//#ifdef _DEBUG	// DX폰트 느린데..
				this->UpdateFrameRate();
				RECT rc = {10, 10, 0, 0};
				CHAR strFPS[256] = {0};
				sprintf( strFPS, _T("%.02f fps"), m_fFPS);
				m_pFPSFont->DrawText( NULL, strFPS, -1, &rc, DT_NOCLIP, D3DXCOLOR( 1.0f, 1.0f, 0.0f, 1.0f ));
//#endif
				m_pD3DDevice->EndScene();
				m_pD3DDevice->Present( NULL, NULL, NULL, NULL );
			}
        }
    }
    return true;
}

LRESULT WINAPI CD3DApp::MessageHandler(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
	switch(Message) 
	{
	case WM_LBUTTONDOWN:
		this->OnLButtonDown(LOWORD(lParam),HIWORD(lParam),Message);
		break;
	case WM_LBUTTONUP:
		this->OnLButtonUp(LOWORD(lParam),HIWORD(lParam),Message);
		break;
	case WM_RBUTTONDOWN:
		this->OnRButtonDown(LOWORD(lParam),HIWORD(lParam),Message);
		break;
	case WM_RBUTTONUP:
		this->OnRButtonUp(LOWORD(lParam),HIWORD(lParam),Message);
		break;
	case WM_DESTROY:
	case WM_CLOSE:
	case WM_QUIT:
		{
			int a = 20;
			a=30;
		}
		break;
	case WM_KEYDOWN:
		switch(wParam)
		{
		case VK_ESCAPE:
			if( MessageBox(hWnd,"종료 하시겠습니까?","종료",MB_YESNO) == IDYES )
			{
				PostQuitMessage(0);
			}
			break;
		}
		break;
	}

	return DefWindowProc(hWnd, Message, wParam, lParam);
}

HRESULT CD3DApp::InitD3DDevice()
{
	HRESULT hr = 0;

	// Step 1: Create the IDirect3D9 object.

	IDirect3D9* d3d9 = 0;
    d3d9 = Direct3DCreate9(D3D_SDK_VERSION);

    if( !d3d9 )
	{
		::MessageBox(0, "Direct3DCreate9() - FAILED", 0, 0);
		return false;
	}

	// Step 2: Check for hardware vp.

/*	D3DCAPS9 caps;
	d3d9->GetDeviceCaps(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &caps);

	int vp = 0;
	if( caps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT )
	{
		vp = D3DCREATE_HARDWARE_VERTEXPROCESSING;
	}
	else
	{
		vp = D3DCREATE_SOFTWARE_VERTEXPROCESSING;
	}*/

	// Step 3: Fill out the D3DPRESENT_PARAMETERS structure.
 
	D3DPRESENT_PARAMETERS d3dpp;
	ZeroMemory( &d3dpp, sizeof( d3dpp ) );
	d3dpp.BackBufferWidth            = m_dwScreenWidth;
	d3dpp.BackBufferHeight           = m_dwScreenHeight;
	d3dpp.BackBufferFormat           = D3DFMT_A8R8G8B8;
	d3dpp.BackBufferCount            = 1;
	d3dpp.MultiSampleType            = D3DMULTISAMPLE_NONE;
	d3dpp.MultiSampleQuality         = 0;
	d3dpp.SwapEffect                 = D3DSWAPEFFECT_DISCARD; 
	d3dpp.hDeviceWindow              = m_hWndMain;
	d3dpp.Windowed                   = true;
	d3dpp.EnableAutoDepthStencil     = true; 
	d3dpp.AutoDepthStencilFormat     = D3DFMT_D24S8;
	d3dpp.Flags                      = 0;
	d3dpp.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
	d3dpp.PresentationInterval       = D3DPRESENT_INTERVAL_IMMEDIATE;

	// Step 4: Create the device.

	D3DDEVTYPE DeviceType(D3DDEVTYPE_HAL);

	hr = d3d9->CreateDevice(
		D3DADAPTER_DEFAULT, // primary adapter
		DeviceType,         // device type
		m_hWndMain,               // window associated with device
		m_vertexProcessing,                 // vertex processing
	    &d3dpp,             // present parameters
	    &m_pD3DDevice);            // return created device

	if( FAILED(hr) )
	{
		// try again using a 16-bit depth buffer
		d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
		
		hr = d3d9->CreateDevice(D3DADAPTER_DEFAULT,	DeviceType, m_hWndMain,
								m_vertexProcessing, &d3dpp, &m_pD3DDevice);

		if( FAILED(hr) )
		{
			d3d9->Release(); // done with d3d9 object
			::MessageBox(0, "CreateDevice() - FAILED", 0, 0);
			return false;
		}
	}

	d3d9->Release(); // done with d3d9 object
	
    // 다이렉트 객체 
   /* LPDIRECT3D9 pD3D = Direct3DCreate9( D3D_SDK_VERSION );
	if(NULL == pD3D ) return E_FAIL;

	// 데탑
    pD3D->GetAdapterDisplayMode( D3DADAPTER_DEFAULT, &m_DesktopMode );
    
    ZeroMemory( &m_d3dpp, sizeof(m_d3dpp) );
    m_d3dpp.Windowed         = !m_bFullScreen;
    m_d3dpp.BackBufferCount  = 1;
    m_d3dpp.SwapEffect       = D3DSWAPEFFECT_DISCARD;
    m_d3dpp.EnableAutoDepthStencil = TRUE;
    m_d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
    m_d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
    if( m_bFullScreen )
    {
        m_d3dpp.hDeviceWindow    = m_hWndMain;
        m_d3dpp.BackBufferWidth  = m_dwScreenWidth;
        m_d3dpp.BackBufferHeight = m_dwScreenHeight;
        m_d3dpp.BackBufferFormat = m_DesktopMode.Format;
    }
    else
    {
        m_d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;//m_DesktopMode.Format;
    }

	D3DDEVTYPE dwDevType = D3DDEVTYPE_HAL;
    DWORD dwBehaviorFlags = D3DCREATE_HARDWARE_VERTEXPROCESSING;

	// 버텍스는 하드웨어 버텍스프로세싱으로 만든 뒤, 실패하면 소프트웨어로 만든다.
	if(FAILED(pD3D->CreateDevice(D3DADAPTER_DEFAULT, dwDevType, m_hWndMain, 
									dwBehaviorFlags,&m_d3dpp, &m_pD3DDevice)))
	{
		OutputDebugString("[Failed] CreateDevice HAL 장치 실패\n");
		OutputDebugString("[Loading] SoftWare_VertexProcessing 생성시도\n");
		if(FAILED(pD3D->CreateDevice(D3DADAPTER_DEFAULT, dwDevType, m_hWndMain,
								D3DCREATE_SOFTWARE_VERTEXPROCESSING,
								&m_d3dpp, &m_pD3DDevice)))
			{OutputDebugString("[Failed] CreateDevice SoftVertex 장치실패\n");return E_FAIL;}
	}
   
	pD3D->Release();*/
    return S_OK;
}
