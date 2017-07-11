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

	// ��� �ʱ�ȭ~
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
		OutputDebugString("[����] m_pd3dDevice �� �������� �ʾ���");
		return E_FAIL;
	}
	
	// �׸������� �˾ƺ���.
	// ����/���� ���̸� ���
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
	{ return E_FAIL; } // ���ؽ� ���� ����

	SPRITE_VERTEX *pVertices;

	m_pVertexBuffer->Lock(0, 4*sizeof(SPRITE_VERTEX),(void **)&pVertices,0);

	// ��ǥ ����
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
	// �׸��� �������� ����� �Ǿ� �־, ���� ��ܿ� ���߱� ����
	// ���� ũ���� �ݾ� ������

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
	// ���⼭ �� ��� �� �������� ��� ����
	//-=-=-=-=-=-=-=-=-=-=--=-=-==-=-=-=-=-=-=-=-=-
	// �Ʒ����� �並 �ٲٱ� ������ ,
	// ���� ���õ� �並 ��� �� �־� �Ѵ�.
	D3DVIEWPORT9 TempViewport;
	m_pd3dDevice->GetViewport(&TempViewport);

	// �ٸ� ��� �������� �ʱ� ���ؼ�,
	// �並 ���� ��ķ� ���� ������ѵ�
	D3DXMATRIX mView;
	D3DXMatrixIdentity(&mView);
	m_pd3dDevice->SetTransform(D3DTS_VIEW, &mView);

	// �ȼ������� ���������� ���������� �����. 
	// �������� ���� ������� ����
	RECT rc;
	GetClientRect(m_hWnd,&rc);
	D3DXMatrixOrthoOffCenterLH(&m_Pixel,
								0,
								(float) rc.right,
								(float)rc.bottom,
								0,
								0.0f, 1000.0f);

	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// �ؽ�ó�� ���ĺ��� ��Ų��.
	//---------------------------------------------------------------------
	// �����׽�Ʈ�� ���� ���İ� = TextureAlpha * TextureFactorAlpha
	// ���ؽ����ۿ� ������ �����Ƿ� TextureFactor�� ����Ѵ�.
	m_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
	m_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TFACTOR );
	m_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_TEXTURE );
	//----------------------------------------------------------------------
	// ���ĺ��� On
	m_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	//----------------------------------------------------------------------
	// ������ �ΰ��� ���´�.
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
	// ���ĺ���
	m_pd3dDevice->SetRenderState(D3DRS_TEXTUREFACTOR, D3DCOLOR_ARGB(dwAlpha,255,255,255));
	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

	m_pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, FALSE );
	m_pd3dDevice->SetRenderState(D3DRS_FOGENABLE, FALSE );		// ���� off
	m_pd3dDevice->SetRenderState(D3DRS_LIGHTING, FALSE);		// ����Ʈ off
	m_pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);	// �ø� off
	m_pd3dDevice->SetRenderState(D3DRS_ZENABLE, FALSE);			// Z���� 

	// �ȼ������� �������� ����
	m_pd3dDevice->SetTransform(D3DTS_PROJECTION, &m_Pixel);
	// ���⼭ �ؽ�ó
	m_pd3dDevice->SetTexture(0,pTexture.pTex);
	m_pd3dDevice->SetStreamSource(0, m_pVertexBuffer, 0, sizeof(SPRITE_VERTEX));

	m_pd3dDevice->SetFVF(D3DFVF_SPRITEVERTEX);
	
	m_pd3dDevice->SetTransform(D3DTS_WORLD, &(m_Scaling * m_Rotation * m_Translation));
	m_pd3dDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);

	//----------------------------------------------------------------------
	//	���� �������� ����
	//----------------------------------------------------------------------
	// ����ߴ� ����Ʈ�� �ٽ� ����
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
//	�� �� �� �� �� 
//===================================================================================//

// Fill �����ؼ� ���Ǵ� �Լ���. �ٸ��������� ���� ����
VOID WINAPI ColorFill(D3DXVECTOR4* pOut, const D3DXVECTOR2* pTexCoord,const D3DXVECTOR2* pTexelSize, LPVOID pData)
{
	DWORD p = (DWORD)pData;

	BYTE a = (p>>24);
	BYTE r = (p>>16);
	BYTE g = (p>>8);
	BYTE b = p;
	*pOut = D3DXVECTOR4( r,g,b,a );
}

// ���İ� ��� : dwColor �μ��� D3DCOLOR_ARGB( A, R, G ,B ); �� �ָ� ��.
void CDrawEx::CreateFillRectangle(SPRITE_TEXTURE *pOut, int nWidth, int nHeight, DWORD dwColor)
{
	LPDIRECT3DTEXTURE9 pTexture=NULL;

	if(FAILED( D3DXCreateTexture(m_pd3dDevice,nWidth, nHeight,
						D3DX_DEFAULT,0,D3DFMT_UNKNOWN,
						D3DPOOL_MANAGED, &pTexture) ))
	{
		OutputDebugString("FillRectangle�Լ� ���� / D3DXCreateTexutre Failed");
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
						D3DPOOL_MANAGED, &pTexture) ))		// �ؽ��� ���� ���ؼ�.. D3DPOOL_MANAGED ���..
	{
		OutputDebugString("FillRectangle�Լ� ���� / D3DXCreateTexutre Failed");
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
