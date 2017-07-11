//File. CDrawEx.cpp

#include "StdAfx.h"
#include "DrawEx.h"

CDrawEx::CDrawEx()
{
	m_pVertexBuffer=NULL;
	m_pd3dDevice=NULL;
}

CDrawEx::CDrawEx( LPDIRECT3DDEVICE9 pd3dDevice, HWND hWnd)
{
	m_hWnd = hWnd;
	m_pVertexBuffer=NULL;
	m_pd3dDevice=NULL;
	InitDevice(pd3dDevice);
	CreateSpriteBuffer();
}

CDrawEx::~CDrawEx()
{
	Release();
}

void CDrawEx::Release()
{
	SAFE_RELEASE( m_pVertexBuffer );
}

HRESULT CDrawEx::InitDevice(LPDIRECT3DDEVICE9 pd3dDevice)
{
	m_pd3dDevice = pd3dDevice;

	// 행렬 초기화~
	D3DXMatrixIdentity(&m_Translation);
	D3DXMatrixIdentity(&m_Rotation);
	D3DXMatrixIdentity(&m_Scaling);

	return S_OK;
}


HRESULT CDrawEx::LoadImageFile(char *FileName,SPRITE_TEXTURE *ppTexture, D3DCOLOR KeyColor)
{

//	DWORD m_dwWidth;
//	DWORD m_dwHeight;

	if(m_pd3dDevice==NULL)
	{
		OutputDebugString("[실패] m_pd3dDevice 가 생성되지 않았음");
		return E_FAIL;
	}
	
	// 그림정보를 알아보자.
	// 가로/세로 길이를 백업
	D3DXIMAGE_INFO Temp;
	D3DXGetImageInfoFromFile(FileName,&Temp);
	ppTexture->m_iHeight = Temp.Height;
	ppTexture->m_iWidth = Temp.Width ;
	
	
	if(FAILED( D3DXCreateTextureFromFileEx(m_pd3dDevice,FileName,
		D3DX_DEFAULT,D3DX_DEFAULT,D3DX_DEFAULT,0,
		D3DFMT_UNKNOWN,D3DPOOL_MANAGED,
		D3DX_FILTER_TRIANGLE|D3DX_FILTER_MIRROR,
		D3DX_FILTER_TRIANGLE|D3DX_FILTER_MIRROR,KeyColor, 
		NULL, NULL, &(ppTexture->pTex) ) ))
	{
		return E_FAIL;
	}
	
/*		D3DXCreateTexture(m_pd3dDevice,
							Temp.Width,
							Temp.Height,
							D3DX_DEFAULT,
							0,
							D3DFMT_A8R8G8B8,
							MEMORY_POOL,
							&ppTexture->pTex);

		LPDIRECT3DSURFACE9 psurface = NULL;
		ppTexture->pTex->GetSurfaceLevel(0,&psurface);

		D3DSURFACE_DESC desc;
		psurface->GetDesc(&desc);

		RECT rect = {0,0,desc.Width, desc.Height};

		D3DXLoadSurfaceFromFile(psurface,
									0,
									0,
									FileName,
									&rect,
									D3DX_FILTER_NONE,
									D3DCOLOR_XRGB(255,0,255), &Temp);

		psurface->Release();*/	
		
	return S_OK;

	/**/
}

HRESULT CDrawEx::CreateSpriteBuffer()
{
	if(FAILED( m_pd3dDevice->CreateVertexBuffer( 4 * sizeof(SPRITE_VERTEX), 0, 
									D3DFVF_SPRITEVERTEX, D3DPOOL_MANAGED, 
									&m_pVertexBuffer,NULL) ))
	{ return E_FAIL; } // 버텍스 생성 실패

	SPRITE_VERTEX *pVertices;

	m_pVertexBuffer->Lock(0, 4*sizeof(SPRITE_VERTEX),(void **)&pVertices,0);

	// 좌표 셋팅
	pVertices[0].x = -0.5f;		pVertices[0].y = -0.5f;		pVertices[0].z =  1.0f;
	pVertices[0].v = 0.0f;		pVertices[0].u =  0.0f;

	pVertices[1].x = -0.5f;		pVertices[1].y = 0.5f;		pVertices[1].z = 1.0f;
	pVertices[1].v = 1.0f;		pVertices[1].u = 0.0f;

	pVertices[2].x = 0.5f;		pVertices[2].y = -0.5f;		pVertices[2].z = 1.0f;
	pVertices[2].v = 0.0f;		pVertices[2].u = 1.0f;

	pVertices[3].x = 0.5f;		pVertices[3].y = 0.5f;		pVertices[3].z = 1.0f;
	pVertices[3].v = 1.0f;		pVertices[3].u = 1.0f;

	m_pVertexBuffer->Unlock();

	return S_OK;
}

void CDrawEx::DrawSpriteEx(SPRITE_TEXTURE &pTexture, float x, float y,DWORD dwAlpha,
						   DWORD dwWidth, DWORD dwHeight,
						   float AngleX, float AngleY, float AngleZ)
{
	if(dwWidth==0) dwWidth = pTexture.m_iWidth;
	if(dwHeight==0) dwHeight = pTexture.m_iHeight;
	// x+(dwWidth/2), y+(dwHeight/2)
	// 그림에 기준점이 가운데로 되어 있어서, 왼쪽 상단에 맞추기 위해
	// 각각 크기의 반씩 더해줌

	float Zdist=0;


	if(AngleX > 0 || AngleY > 0 || AngleZ >0)
	{
		if( dwWidth > dwHeight )
			Zdist = (float)dwWidth/2;
		else
			Zdist = (float)dwHeight/2;
	}
	
	TranslateSprite(x+(dwWidth/2),y+(dwHeight/2),Zdist);
	RotateSprite(AngleX, AngleY, AngleZ, dwWidth, dwHeight);//,AngleY,AngleZ);
	ScaleSprite(dwWidth,dwHeight);

	DrawSprite(pTexture, dwAlpha);
}

void CDrawEx::DrawSprite(SPRITE_TEXTURE &pTexture,DWORD dwAlpha)
{
	//=-=-=-=-=-===-=--==-=-==-=-=-=-=-=---=-=-=-=-
	// 여기서 뷰 백업 후 직교투영 행렬 셋팅
	//-=-=-=-=-=-=-=-=-=-=--=-=-==-=-=-=-=-=-=-=-=-
	// 아래에서 뷰를 바꾸기 때문에 ,
	// 현재 셋팅된 뷰를 백업 해 둬야 한다.
	D3DVIEWPORT9 TempViewport;
	m_pd3dDevice->GetViewport(&TempViewport);

	// 다른 뷰와 연관되지 않기 위해서,
	// 뷰를 단일 행렬로 만들어서 적용시켜둠
	D3DXMATRIX mView;
	D3DXMatrixIdentity(&mView);
	m_pd3dDevice->SetTransform(D3DTS_VIEW, &mView);

	// 픽셀기준의 직교투영한 프로젝션을 만든다. 
	// 기준점은 왼쪽 상단으로 맞춤
	RECT rc;
	GetClientRect(m_hWnd,&rc);
	D3DXMatrixOrthoOffCenterLH(&m_Pixel,
								0,
								(float) rc.right,
								(float)rc.bottom,
								0,
								0.0f, 1000.0f);

	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// 텍스처를 알파블렌딩 시킨다.
	//---------------------------------------------------------------------
	// 알파테스트에 사용될 알파값 = TextureAlpha * TextureFactorAlpha
	// 버텍스버퍼에 색상값이 없으므로 TextureFactor를 사용한다.
	m_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
	m_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TFACTOR );
	m_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_TEXTURE );
	//----------------------------------------------------------------------
	// 알파블랜딩 On
	m_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	//----------------------------------------------------------------------
	// 적당히 두개를 섞는다.
	m_pd3dDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	m_pd3dDevice->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_INVSRCALPHA);
	//----------------------------------------------------------------------
	m_pd3dDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
	m_pd3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );

//	m_pd3dDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_POINT );
//	m_pd3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_POINT );

//	m_pd3dDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_NONE );
//	m_pd3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_NONE );

	//----------------------------------------------------------------------
	// 알파블렌딩
	m_pd3dDevice->SetRenderState(D3DRS_TEXTUREFACTOR, D3DCOLOR_ARGB(dwAlpha,255,255,255));
	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

	m_pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, FALSE );
	m_pd3dDevice->SetRenderState(D3DRS_FOGENABLE, FALSE );		// 포그 off
	m_pd3dDevice->SetRenderState(D3DRS_LIGHTING, FALSE);		// 라이트 off
	m_pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);	// 컬링 off
	m_pd3dDevice->SetRenderState(D3DRS_ZENABLE, FALSE);			// Z버퍼 

	// 픽셀기준의 프로젝션 적용
	m_pd3dDevice->SetTransform(D3DTS_PROJECTION, &m_Pixel);
	// 여기서 텍스처
	m_pd3dDevice->SetTexture(0,pTexture.pTex);
	m_pd3dDevice->SetStreamSource(0, m_pVertexBuffer, 0, sizeof(SPRITE_VERTEX));

	m_pd3dDevice->SetFVF(D3DFVF_SPRITEVERTEX);
	
	m_pd3dDevice->SetTransform(D3DTS_WORLD, &(m_Scaling * m_Rotation * m_Translation));
	m_pd3dDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);

	//----------------------------------------------------------------------
	//	원래 설정으로 복구
	//----------------------------------------------------------------------
	// 백업했던 뷰포트를 다시 셋팅
	m_pd3dDevice->SetViewport(&TempViewport);
}

void CDrawEx::RotateSprite(float AngleX, float AngleY, float AngleZ, float x, float y)
{
	D3DXMatrixRotationYawPitchRoll(&m_Rotation, AngleY, AngleX, AngleZ);
}

void CDrawEx::TranslateSprite(float x, float y, float z)
{
	D3DXMatrixTranslation(&m_Translation,x,y,z);
}

void CDrawEx::ScaleSprite(DWORD dwWidth, DWORD dwHeight)
{
	D3DXMatrixScaling(&m_Scaling,(float)dwWidth,(float)dwHeight, 1.0f);
}


//===================================================================================//
//	그 리 기 함 수 
//===================================================================================//

// Fill 관련해서 사용되는 함수다. 다른곳에서는 쓰지 마삼
VOID WINAPI ColorFill(D3DXVECTOR4* pOut, const D3DXVECTOR2* pTexCoord,const D3DXVECTOR2* pTexelSize, LPVOID pData)
{
	DWORD p = (DWORD)pData;

	BYTE a = (p>>24);
	BYTE r = (p>>16);
	BYTE g = (p>>8);
	BYTE b = p;
	*pOut = D3DXVECTOR4( r,g,b,a );
}

// 알파값 사용 : dwColor 인수에 D3DCOLOR_ARGB( A, R, G ,B ); 로 주면 됨.
void CDrawEx::CreateFillRectangle(SPRITE_TEXTURE *pOut, int nWidth, int nHeight, DWORD dwColor)
{
	LPDIRECT3DTEXTURE9 pTexture=NULL;

	if(FAILED( D3DXCreateTexture(m_pd3dDevice,nWidth, nHeight,
						D3DX_DEFAULT,0,D3DFMT_UNKNOWN,
						D3DPOOL_MANAGED, &pTexture) ))
	{
		OutputDebugString("FillRectangle함수 실패 / D3DXCreateTexutre Failed");
		return;
	}
	pOut->pTex		= pTexture;
	pOut->m_iWidth	= nWidth;
	pOut->m_iHeight	= nHeight;

	D3DXFillTexture(pTexture,ColorFill,(VOID*)dwColor);
}

void CDrawEx::CreateFillRectangleEx(SPRITE_TEXTURE *pOut, int nWidth, int nHeight, DWORD dwColor, UINT MipLevels)
{
	LPDIRECT3DTEXTURE9 pTexture=NULL;

	if(FAILED( D3DXCreateTexture(m_pd3dDevice,nWidth, nHeight,
						MipLevels,0,D3DFMT_A8R8G8B8,
						D3DPOOL_MANAGED, &pTexture) ))		// 텍스쳐 락을 위해서.. D3DPOOL_MANAGED 사용..
	{
		OutputDebugString("FillRectangle함수 실패 / D3DXCreateTexutre Failed");
		return;
	}
	pOut->pTex		= pTexture;
	pOut->m_iWidth	= nWidth;
	pOut->m_iHeight	= nHeight;

	D3DXFillTexture(pTexture,ColorFill,(VOID*)dwColor);
}

void CDrawEx::FillRectangle(int nX, int nY, int nWidth, int nHeight, DWORD dwColor/* =0 */,float z, float fAngle)
{
	SPRITE_TEXTURE Texture;	
	this->CreateFillRectangle(&Texture,nWidth,nHeight,dwColor);

	CDrawEx::DrawSpriteEx(Texture,nX,nY,255, nWidth,nHeight,0.0f,0.0f,0.0f);
}
