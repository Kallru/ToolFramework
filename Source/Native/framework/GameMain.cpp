//------------------------------------------------------------------------------
// File. GameMain.cpp
//------------------------------------------------------------------------------

#include "StdAfx.h"

CGameMain::CGameMain( HWND hWnd, DWORD dwWidth, DWORD dwHegiht, const string& dataPath, int vetexProcessing )
:CD3DApp( hWnd, dwWidth, dwHegiht, vetexProcessing )
, m_dataPath( dataPath )
{
	this->Init();
}
CGameMain::~CGameMain()
{
	SAFE_DELETE(m_pSkydome);
	SAFE_DELETE(m_pCamera);
	SAFE_DELETE(m_pTimeMove);
	SAFE_DELETE_ARRAY(m_pFrustum);

	DWORD dwSize = m_listObject.size();
	int i;
	for(i=0; i<dwSize; ++i)
	{
		SAFE_DELETE(m_listObject[i]);
	}
	m_listObject.clear();
}

void CGameMain::Init()
{
	m_pSkydome	= NULL;
	m_pCamera	= NULL;
	m_pTimeMove = NULL;
	m_pPlayer	= NULL;

	m_pTerrain	= NULL;
	m_pFrustum	= NULL;
	m_cpRenderer= NULL;
	m_pPick		= NULL;
	m_pDraw		= NULL;
	m_pTarTex	= NULL;
	
	m_dwLastTime = 0;
	m_dwCurTime	 = 0;
	m_ptMouse.x  = 0;
	m_ptMouse.y  = 0;

	m_mouseLock = false;

	m_fAspect	 = (float)m_dwScreenWidth / (float)m_dwScreenHeight;
}

bool CGameMain::LoadScene()
{
	g_Utility.m_rootPath = m_dataPath;

	m_pSkydome = new CXFile;
	if( FAILED( m_pSkydome->Create( g_Utility.GetAssetPath( "skydome.x" ), m_pD3DDevice,false) ))
		return false;
	if( FAILED( m_pSkydome->CreateTexture( g_Utility.GetAssetPath( "Env_Sky.jpg" ) )) )
		return false;

	m_pDraw = new CDrawEx(m_pD3DDevice,m_hWndMain);
	m_pTarTex = new SPRITE_TEXTURE;
	m_pDraw->LoadImageFile( g_Utility.GetAssetPath( "target.png" ) ,m_pTarTex);

	m_listObject.reserve(20);

	// ������Ʈ ���� (�Ŵ��� Ŭ������ �ʿ��ϱ� ��)
	// �׺��� ������Ʈ Ŭ���� ��ü�� �ʿ��ѵ� ����; �� ������Ʈ�� ��Ʈ���� �����ϳ�..
	// ���� ����⿣ �ð��� �ʹ� ���� �ɸ��µ�... txt�� �ִ°͵� �Žñ� �ϰ�...-_-.......����� ����
	CXFile *pObject = new CXFile;
	pObject->Create( g_Utility.GetAssetPath( "testTorus.X" ),m_pD3DDevice,true);
	pObject->SetTranslation(128,13,128);
//	pObject->SetScaling(0.01f,0.01f,0.01f);
	m_listObject.push_back(pObject);

	pObject = new CXFile;
	pObject->Create( g_Utility.GetAssetPath( "Model1.X" ),m_pD3DDevice,false);
	pObject->SetTranslation(100,10,128);
//	pObject->SetScaling(0.01f,0.01f,0.01f);
	m_listObject.push_back(pObject);

	pObject = new CXFile;
	pObject->Create( g_Utility.GetAssetPath( "Teapot.X" ),m_pD3DDevice,true);
	pObject->SetTranslation(100,2,80);
//	pObject->SetScaling(0.01f,0.01f,0.01f);
	m_listObject.push_back(pObject);

	pObject = new CXFile;
	pObject->Create( g_Utility.GetAssetPath( "Box.X" ),m_pD3DDevice,false);
	pObject->SetTranslation(80,5,120);
//	pObject->SetScaling(0.01f,0.01f,0.01f);
	m_listObject.push_back(pObject);

	// ���� ����
	m_pTerrain = new ZTerrain;

	LPSTR lpTexPath[4] = { g_Utility.GetAssetPath( "Tile\\Tile5.png" ), g_Utility.GetAssetPath( "lightmap.tga" ),"","" };

	if(FAILED(m_pTerrain->Create(m_pD3DDevice,&D3DXVECTOR3(1.f,1.f,1.f), g_Utility.GetAssetPath( "map1282.bmp" ),lpTexPath)) )
	{
		assert(false);
		return false;
	}

	// ù��° ���������� ���� ������, �ι�°�� ��ŷ�˻��
	m_pFrustum = new ZFrustum[2];
	m_pCamera	= new CCamera;
	m_pTimeMove = new CTimer;
	m_pPlayer	= new CPlayer;
	m_pPlayer->Create(m_pD3DDevice,m_pCamera,D3DXVECTOR3(0,0,0),m_pTerrain);

	// ��ŷ ����
	m_pPick = new CPicking();
	//m_pPick->Create(m_pTerrain);	// ���� ���� �����Ѵ�

	// Cartoon
	m_cpRenderer = new CRender_Cartoon;
	DWORD dwFVF = pObject->GetFVF();
	((CRender_Cartoon *)m_cpRenderer)->CreateShaders(m_pD3DDevice,m_pCamera,
		g_Utility.GetAssetPath( "Sillhouette.vs" ), 
		g_Utility.GetAssetPath( "Surface.vs" ), 
		g_Utility.GetAssetPath( "toon.dds" ), dwFVF );

	return true;
}

bool CGameMain::Keydown()
{
	if( KEYDOWN('W') )
	{
		m_pPlayer->Move('W');
	}
	if( KEYDOWN('S') )
	{
		m_pPlayer->Move('S');
	}
	if( KEYDOWN('A') )
	{
		m_pPlayer->Move('A');
	}
	if( KEYDOWN('D') )
	{
		m_pPlayer->Move('D');
	}	
	if( KEYDOWN( VK_CONTROL ) )
	{
		m_mouseLock = !m_mouseLock;
	}

	return true;
}

bool CGameMain::MouseMove()
{
	// if( m_hWndMain != GetFocus() )
	// {	return false;	}

	if( !m_mouseLock )
	{
		return false;
	}

	POINT pt;
	float fDelta = 0.0007f;	// ���콺 ����
	GetCursorPos(&pt);

	int dx = pt.x - m_ptMouse.x;
	int dy = pt.y - m_ptMouse.y;

	float AngleX,AngleY;
	AngleX = dx*fDelta;
	AngleY = dy*fDelta;

	// ī�޶� ȸ��
	if( m_pPlayer )
	{
		m_pPlayer->RotateX(AngleY);
		m_pPlayer->RotateY(AngleX);
	}
	
	RECT rc;
	GetClientRect(m_hWndMain,&rc);

	pt.x = (rc.right - rc.left)/2;
	pt.y = (rc.bottom - rc.top)/2;

	ClientToScreen(m_hWndMain,&pt);
	SetCursorPos(pt.x,pt.y);
	m_ptMouse.x = pt.x;
	m_ptMouse.y = pt.y;

	// Ŀ�� ���߱�
	//ShowCursor(false);
	//ShowCursor(true);
	return true;
}

bool CGameMain::FrameMove()
{
	this->MouseMove();
	this->Keydown();

	{
		RECT rc;
		::GetClientRect( m_hWndMain, &rc );
		m_dwScreenWidth = rc.right - rc.left;
		m_dwScreenHeight = rc.bottom - rc.top;
		m_fAspect	 = (float)m_dwScreenWidth / (float)m_dwScreenHeight;
	}

	m_dwCurTime = timeGetTime();
	DWORD dwElapsed = m_dwCurTime - m_dwLastTime;
	m_dwLastTime = m_dwCurTime;

	float fDelta = (float)dwElapsed/1000;
	CTimer::UpdateAll(fDelta);

	if( m_cpRenderer ) m_cpRenderer->FrameMove();

	POINT ptCursor;
	ptCursor.x = m_dwScreenWidth/2;
	ptCursor.y = m_dwScreenHeight/2;

	D3DXVECTOR3 vRayOrig;
	D3DXVECTOR3 vRayDir;

	m_pPick->Unprojection(m_pD3DDevice, m_pCamera, &ptCursor,&vRayOrig,&vRayDir);

	// ������ ��ŷ
	CPicking Terrain;
	Terrain.Create(m_pTerrain)->TriangleIntersectTerrain(&vRayOrig,&vRayDir);

	// ������Ʈ ��ŷ �ʿ�! (�ø��� ������)
	CPicking Object;

	// �ӵ��� ����̶� �ø��� ���� ���͸� ����  ������ [] ���� ���Ͱ� ����
	vector<CXFile*>::iterator iter = m_listObject.begin();
	vector<CXFile*>::iterator itEnd = m_listObject.end();
	for(; iter != itEnd; ++iter)
	{
		CPicking Temp;
		Temp.Create( (*iter) )->TriangleIntersect(&vRayOrig,&vRayDir);

		if( Temp.m_fDist < Object.m_fDist )
		{	Object = Temp;	}
	}

	// ����
	if( Terrain.m_fDist < Object.m_fDist )
	{	*m_pPick = Terrain;	}
	else {	*m_pPick = Object;	}

	// �÷��̾ ���� ��ŷ�� �����͸� �ѱ��.
	m_pPlayer->Collision(m_pPick);
	m_pPlayer->FrameMove(fDelta);
	return true;
}

bool CGameMain::Render()
{
	//------------------------------------------------------------------------//
	// Light : �� ���� ���ε��� ���ִ� ����� ������ �� �� �ִ�.(�����ϴ°� �� ������)
	//------------------------------------------------------------------------//
	this->SetupLight();

	D3DXMATRIX matWorld;
	D3DXMatrixIdentity( &matWorld );
	m_pD3DDevice->SetTransform( D3DTS_WORLD, &matWorld );

	D3DXMATRIX *pMatView = m_pCamera->GetViewMatrix();
	m_pD3DDevice->SetTransform( D3DTS_VIEW, pMatView );

	//------------------------------------------------------------------------//
	// Sky dome
/**///------------------------------------------------------------------------//
	m_pCamera->SetProjection(1.3f, m_fAspect, 0.01f, 1000.0f );
	m_pD3DDevice->SetTransform( D3DTS_PROJECTION, &m_pCamera->GetProjMatrix());

	D3DXVECTOR3 vEyePt = m_pCamera->GetEyePt();
	m_pSkydome->SetTranslation(vEyePt.x,vEyePt.y - 400,vEyePt.z);

	m_pD3DDevice->SetRenderState( D3DRS_LIGHTING,     FALSE );
	m_pD3DDevice->SetRenderState( D3DRS_SRCBLEND,     D3DBLEND_ONE );
	m_pD3DDevice->SetRenderState( D3DRS_DESTBLEND,    D3DBLEND_ZERO );
	m_pD3DDevice->SetRenderState( D3DRS_ZENABLE,      FALSE );
	m_pD3DDevice->SetRenderState( D3DRS_ZWRITEENABLE, FALSE );
	m_pD3DDevice->SetRenderState( D3DRS_FOGENABLE,    FALSE );
	m_pD3DDevice->SetRenderState( D3DRS_CULLMODE,	  D3DCULL_CCW );
	
	m_pD3DDevice->SetSamplerState(0, D3DSAMP_MAGFILTER,D3DTEXF_LINEAR);
	m_pD3DDevice->SetSamplerState(1, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
	m_pSkydome->DrawSubset();
/**/

	//-------------------------------------------------------------------------//
	// �⺻ �� ����
	//-------------------------------------------------------------------------//
	//D3DXMATRIX matWorld;
	D3DXMatrixIdentity( &matWorld );
	m_pD3DDevice->SetTransform( D3DTS_WORLD, &matWorld );

	pMatView = m_pCamera->GetViewMatrix();
	m_pD3DDevice->SetTransform( D3DTS_VIEW, pMatView );

	D3DXMATRIX * pMatProj = m_pCamera->SetProjection( D3DX_PI/4, m_fAspect, 0.01f, 5000.0f );
	m_pD3DDevice->SetTransform( D3DTS_PROJECTION, pMatProj);

	// �������� ����
	if( m_pFrustum ) 
	{
		D3DXMATRIX matProj;
		D3DXMatrixPerspectiveFovLH( &matProj, D3DX_PI/4, m_fAspect, 0.1f, 50.f );
		D3DXMATRIXA16 matFru = (*pMatView) * matProj;
		m_pFrustum[0].Make(&matFru);

		// ���� ��ŷ�� ��������
		D3DXMatrixPerspectiveFovLH( &matProj, 0.0001f, m_fAspect, 0.1f, 50.f );
		matFru = (*pMatView) * matProj;
		m_pFrustum[1].Make(&matFru);
	}
/**/
	// ���� ����
	this->SetupPixelFog(D3DCOLOR_XRGB(131,181,214));

	// ���� ����
	m_pTerrain->Render(m_pFrustum);	// ���ο��� ����Ʈ ���� �ʿ�
	
	// ������Ʈ ����, ����̶� �ӵ� ����� ���� ���͸� ������.
	vector<CXFile*>::iterator iter = m_listObject.begin();
	vector<CXFile*>::iterator itEnd = m_listObject.end();	
	for(; iter != itEnd; ++iter)
	{
		(*iter)->Render(m_cpRenderer);
	}

	// �÷��̾� ����
	m_pPlayer->Render();

	//----------------------------------------------------
	// 2D�� �׸���
	//----------------------------------------------------
	RECT rc;
	GetClientRect(m_hWndMain,&rc);

	float x = (rc.right - rc.left)/2;
	float y = (rc.bottom - rc.top)/2;
	x -= m_pTarTex->m_iWidth/2;
	y -= m_pTarTex->m_iHeight/2;
	m_pDraw->DrawSpriteEx(*m_pTarTex,x,y,120);

	return true;
}

bool CGameMain::SetupLight()
{
	D3DXVECTOR3 vecDir;
	D3DLIGHT9 light;
	ZeroMemory(&light,sizeof(D3DLIGHT9));
	light.Type = D3DLIGHT_DIRECTIONAL;
	light.Position	= D3DXVECTOR3(0,50,0);
	light.Diffuse.r = 1.0f;
	light.Diffuse.g = 1.0f;
	light.Diffuse.b = 1.0f;
	vecDir = D3DXVECTOR3(0,-1,0);
	
	D3DXVec3Normalize((D3DXVECTOR3*)&light.Direction, &vecDir);
	light.Range = 1000.0f;
	m_pD3DDevice->SetLight(0, &light);

	// ��...���� ����� �Ǵ°� �ǹ̻� ���� ������..��..
	m_pD3DDevice->LightEnable(0,TRUE);
	m_pD3DDevice->SetRenderState(D3DRS_AMBIENT, 0x00909090);

	return true;
}

bool CGameMain::SetupPixelFog(DWORD dwColor, DWORD dwMode)
{
	float Start = 3.0f;
    float End   = 50.0f;
	float fDensity	= 0.6f; 
	
	m_pD3DDevice->SetRenderState(D3DRS_FOGENABLE, TRUE);
	m_pD3DDevice->SetRenderState(D3DRS_FOGCOLOR, dwColor);

	m_pD3DDevice->SetRenderState(D3DRS_FOGTABLEMODE, dwMode);

	if(D3DFOG_LINEAR == dwMode)
	{
		m_pD3DDevice->SetRenderState(D3DRS_FOGSTART, *(DWORD *)(&Start));
		m_pD3DDevice->SetRenderState(D3DRS_FOGEND,   *(DWORD *)(&End));

	}
	else
	{	m_pD3DDevice->SetRenderState(D3DRS_FOGDENSITY, *(DWORD *)(&fDensity));	}

	return true;
}

bool CGameMain::OnLButtonDown(int x,int y,UINT nFlag)
{
	// Fire
	m_pPlayer->State(PS_FireStart);
	return true;
}

bool CGameMain::OnLButtonUp(int x,int y,UINT nFlag)
{
	// Fire
	m_pPlayer->State(PS_FireStop);
	return true;
}
