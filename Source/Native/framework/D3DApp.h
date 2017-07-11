//------------------------------------------------------------------------------
// File. D3DApplication.h
//------------------------------------------------------------------------------

#ifndef _D3DAPPLICATION_
#define _D3DAPPLICATION_

#define KEYDOWN(nKey) HIBYTE(GetAsyncKeyState(nKey))
#define KEYUP(nKey) ((GetAsyncKeyState(nKey) & 0x8000) ? 0 : 1)

class CORE_API CD3DApp
{
public:
	CD3DApp( HWND hWnd, DWORD dwWidth, DWORD dwHegiht, int vertexProcessing );
	virtual ~CD3DApp();

	virtual bool LoadScene() = 0;
	virtual bool FrameMove() = 0;
	virtual bool Render() = 0;
	
	virtual bool	Run();
	virtual bool	Stop();

	virtual LRESULT WINAPI MessageHandler(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam);

	// Event
	virtual bool OnLButtonDown(int x,int y,UINT nFlag)	{return true;}
	virtual bool OnLButtonUp(int x,int y,UINT nFlag)	{return true;}
	virtual bool OnRButtonDown(int x,int y,UINT nFlag)	{return true;}
	virtual bool OnRButtonUp(int x,int y,UINT nFlag)	{return true;}

	HWND GetHandle()const { return m_hWndMain; }

private:
	virtual HRESULT Create( HWND hWndParent );

	HRESULT InitD3DDevice();
	VOID UpdateFrameRate();
	//HRESULT CreateShaders();

protected:
	HWND m_hWndMain;
	DWORD m_dwScreenWidth;
	DWORD m_dwScreenHeight;
	DWORD m_dwWindowStyle;
	bool m_bFullScreen;

	D3DDISPLAYMODE	m_DesktopMode;
	D3DPRESENT_PARAMETERS m_d3dpp;

	LPDIRECT3DDEVICE9 m_pD3DDevice;
	DWORD m_dwToonShader;

	float m_fFPS;
	LPD3DXFONT			m_pFPSFont;
	int m_vertexProcessing;

	bool m_bStart;
};

#endif
