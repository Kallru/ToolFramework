// ZTerrain.cpp

#include "stdafx.h"
#include "ZTerrain.h"

ZTerrain::ZTerrain()
{
	m_cxDIB		= 0;
	m_czDIB		= 0;
	m_cyDIB		= 0;
	m_vScale	= D3DXVECTOR3(1.f,1.f,1.f);
	m_pHeightMap= NULL;
	m_pd3dDevice= NULL;
	
	for(int i=0; i<MAX_TERRAIN_TEX; ++i)
	{	m_pTex[i] = NULL;	}

	m_pVB		= NULL;
	m_pIB		= NULL;
	m_nTriangles= 0;
	m_pQuadTree = NULL;
	m_pFace		= NULL;
	m_nCullFaceNum = 0;
}

ZTerrain::~ZTerrain()
{
	this->_Destroy();
}

HRESULT ZTerrain::Create(LPDIRECT3DDEVICE9 pDe, D3DXVECTOR3 *pScale, 
						 LPSTR lpBMPFileName, LPSTR lpTexFile[MAX_TERRAIN_TEX])
{
	m_pd3dDevice = pDe;
	m_vScale = *pScale;

	// ���̸� ����
	if(FAILED( this->_BuildHeightMap(lpBMPFileName) )) 
	{
		this->_Destroy();
		return E_FAIL;
	}

	if(FAILED( this->_LoadTexture(lpTexFile) ))
	{
		this->_Destroy();
		return E_FAIL;
	}

	if(FAILED(this->_CreateVIB() ))
	{
		this->_Destroy();
		return E_FAIL;
	}

	m_pQuadTree = new ZQuadTree(m_cxDIB,m_czDIB);

	if(FAILED(this->_BuildQuadTree() ))
	{
		this->_Destroy();
		return E_FAIL;
	}

	// ���� �ؽ�ó ���� - ���� �ӽ�
	this->AddSplattingTex( g_Utility.GetAssetPath( "Tile\\Alpha1.tga" ), g_Utility.GetAssetPath( "Tile\\Tile4.png" ) );
	this->AddSplattingTex( g_Utility.GetAssetPath( "Tile\\Alpha2.tga" ), g_Utility.GetAssetPath( "Tile\\Tile6.png" ) );
	
	return S_OK;
}

HRESULT ZTerrain::_Destroy()
{
	SAFE_DELETE(m_pQuadTree);
	SAFE_DELETE_ARRAY(m_pHeightMap);
	SAFE_RELEASE(m_pVB);
	SAFE_RELEASE(m_pIB);

	int i;
	for(i=0; i<4; ++i)
	{
		SAFE_RELEASE(m_pTex[i]);
	}

	SAFE_DELETE_ARRAY(m_pFace);
	return S_OK;
}

HRESULT ZTerrain::_LoadTexture(LPSTR lpTexFileName[MAX_TERRAIN_TEX])
{
	int i;
	for(i=0; i<4; ++i)
	{
		if(FAILED(D3DXCreateTextureFromFile(m_pd3dDevice,lpTexFileName[i],&m_pTex[i])) )
		{
			// 0�� �ؽ�ó�� �ƴ϶��, �����ص� ������ ���� �ʴ´�.
			// 0���ؽ�ó�� ���� �⺻ ���̽� �ؽ�ó��, ���� ���Ѵٸ� ������ ���°� ���ڴ�.
			if(0 !=i ) continue;

			assert(false);
			return E_FAIL;
		}
	}
	return S_OK;
}

HRESULT ZTerrain::_BuildHeightMap(LPSTR lpFileName)
{
/**/
	CBmpFile cBmpFile;

	if( FAILED(cBmpFile.LoadFile(lpFileName) ))
	{
		return E_FAIL;
	}

	m_cxDIB = cBmpFile.GetWidth();
	m_czDIB = cBmpFile.GetHeight();

	m_pHeightMap = new TERRAINVERTEX[m_cxDIB * m_czDIB];

	TERRAINVERTEX v;
	int z,x;
	for(z=0; z<m_czDIB; ++z)
	{
		for(x=0; x<m_cxDIB; ++x)
		{
			v.p.x = (float) x * m_vScale.x;
			v.p.z = (float) z * m_vScale.z;

			BYTE aa = cBmpFile.GetData(x,z);

			if(aa>0)
			{
				int a=0;
			}

			v.p.y = (float)aa * 0.1f;

			D3DXVec3Normalize(&v.n, &v.p);

			v.t.x = (float)x / (float)(m_cxDIB - 1);
			v.t.y = (float)z / (float)(m_czDIB - 1);
			m_pHeightMap[x+z*m_czDIB] = v;
		}
	}
/**/
/*
	LPDIRECT3DTEXTURE9 pTexture;
	// ���̸�  "map128.bmp"
	if(FAILED (D3DXCreateTextureFromFileEx(m_pd3dDevice,lpFileName,D3DX_DEFAULT, D3DX_DEFAULT,
		D3DX_DEFAULT,0, D3DFMT_X8R8G8B8, D3DPOOL_MANAGED, 
		D3DX_DEFAULT,D3DX_DEFAULT,0,NULL,NULL, &pTexture) ))
	{
		return E_FAIL;
	}

	D3DSURFACE_DESC ddsd;
	D3DLOCKED_RECT	d3drc;
	
	pTexture->GetLevelDesc(0,&ddsd);
	m_cxDIB = ddsd.Width+1;
	m_czDIB = ddsd.Height+1;

	m_pHeightMap = new TERRAINVERTEX[m_cxDIB * m_czDIB];

	pTexture->LockRect(0, &d3drc, NULL, D3DLOCK_READONLY);

	TERRAINVERTEX v;z
	int z,x;
	for(z=0; z<m_czDIB; ++z)
	{
		for(x=0; x<m_cxDIB; ++x)
		{
			v.p.x = (float) x * m_vScale.x;
			v.p.z = (float) z * m_vScale.z;
			//v.p.x = (float) ((x-m_cxDIB/2) * m_vScale.x);
			//v.p.z = -(float) ((z-m_czDIB/2) * m_vScale.z);
			v.p.y = ((float)(*((LPDWORD)d3drc.pBits+x+z*(d3drc.Pitch/4))&0x000000ff)) / 10.0f * m_vScale.y;
			
			D3DXVec3Normalize(&v.n, &v.p);

			//v.t.x = 0;//x%2? 0:1;
			//v.t.y = 0;//z%2? 0:1;
			v.t.x = (float)x / (float)(m_cxDIB - 1);
			v.t.y = (float)z / (float)(m_czDIB - 1);
			m_pHeightMap[x+z*m_czDIB] = v;
		}
	}

	pTexture->UnlockRect(0);
	pTexture->Release();
	
	/**/
	return S_OK;
}

HRESULT ZTerrain::_BuildQuadTree()
{
	m_pQuadTree->Build( m_pHeightMap );
	return S_OK;
}

HRESULT ZTerrain::_CreateVIB()
{
	// VB
	if(FAILED(m_pd3dDevice->CreateVertexBuffer( m_cxDIB*m_czDIB*sizeof(TERRAINVERTEX), 0,
		TERRAINVERTEX::FVF, D3DPOOL_DEFAULT, &m_pVB, NULL) ))
	{
		this->_Destroy();
		return E_FAIL;
	}

	VOID *pVertices;
	if(FAILED(m_pVB->Lock(0,m_cxDIB*m_czDIB*sizeof(TERRAINVERTEX), (void**)&pVertices, 0)))
	{
		this->_Destroy();
		return E_FAIL;
	}

	memcpy(pVertices,m_pHeightMap,m_cxDIB*m_czDIB*sizeof(TERRAINVERTEX));
	m_pVB->Unlock();

	DWORD dwSize = (m_cxDIB-1)*(m_czDIB-1)* 2;	// �� 2�� ���ϴ°ɱ�

	if(FAILED(m_pd3dDevice->CreateIndexBuffer( dwSize * sizeof(TRIINDEX),0, D3DFMT_INDEX16,
		D3DPOOL_DEFAULT, &m_pIB, NULL )))
	{
		this->_Destroy();
		return E_FAIL;
	}

	// ���۰� �ʹ� ū�� �Ѥ�;
	m_pFace = new TRIINDEX[dwSize];
	ZeroMemory(m_pFace,dwSize * sizeof(TRIINDEX));

	return S_OK;
}

HRESULT ZTerrain::Render( ZFrustum* pFrustum )
{
	LPDWORD pI;
	TRIINDEX *pTemp = NULL;
	if( FAILED(this->IndexLock(&pI)) )
	{
		return E_FAIL;
	}

	m_nTriangles = m_pQuadTree->GenerateIndex(pI, m_pHeightMap, &pFrustum[0]);
	
	this->IndexUnlock();

	// ��ŷ�� ���� �ε����� ����
	DWORD dwSize = (m_cxDIB-1)*(m_czDIB-1)* 2;
	ZeroMemory(m_pFace,dwSize * sizeof(TRIINDEX));
	m_nCullFaceNum = m_pQuadTree->GenerateIndex(m_pFace, m_pHeightMap, &pFrustum[1]);	// ��������1���� �ణ ������ �ִµ�

	D3DMATERIAL9 mtrl;
	ZeroMemory(&mtrl, sizeof(D3DMATERIAL9));
	mtrl.Diffuse.r = mtrl.Ambient.r = 1.0f;
	mtrl.Diffuse.g = mtrl.Ambient.g = 1.0f;
	mtrl.Diffuse.b = mtrl.Ambient.b = 1.0f;
	mtrl.Diffuse.a = mtrl.Ambient.a = 1.0f;
	m_pd3dDevice->SetMaterial(&mtrl);

	// RenderState
	m_pd3dDevice->SetRenderState( D3DRS_LIGHTING,		TRUE);
	m_pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE,	TRUE );
	m_pd3dDevice->SetRenderState( D3DRS_ZENABLE,		TRUE );
	m_pd3dDevice->SetRenderState( D3DRS_CULLMODE,		D3DCULL_CW );
	m_pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE,FALSE );
	m_pd3dDevice->SetRenderState( D3DRS_DITHERENABLE,	FALSE );
	//m_pd3dDevice->SetRenderState( D3DRS_FILLMODE,		D3DFILL_WIREFRAME );
	
	//---------------------------------------------------------------------------------------------------------------------
	//  - �� ���� ���� -
	//  �� 3���� ��Ƽ �н��� �̷���� �ִ�. ��_�� (���н��� ���̰� �ʹ�;;;)
	//	ù��° �н� - ����ؽ�ó�� 2���� ���̽� �ؽ�ó/����Ʈ�ؽ�ó�� �̷�����ִ�.	(�Լ� : _RenderBaseTexture)
	//	�ι�° �н� - ���÷����� �ϴ� �н�, ���ĸ�/�ش� �̹����ؽ�ó�� �̷�����ִ�.(�Լ� : _RenderAlphaTexture)
	//	����° �н� - ���÷����� ���� �ٷ� ���� �̷������ ����Ʈ �ؽ�ó�� ���������� ������.(�ι�° �н� �ٷ� �ؿ��� ��)
	//	- ����Ʈ���� ������ �н��� �ѹ� �� �ϴ°�, ���÷����� �κп� ����Ʈ���� �����ϱ� ����..
	//---------------------------------------------------------------------------------------------------------------------
	
	if(FAILED( this->_RenderBaseTexture() ))
	{
		assert(false);
		return E_FAIL;
	}

	// ���� ������ Fail �ɼ� �ִ�. (���ĸ��� �Ⱦ����� �����ϱ�)
	// �� �Լ����� �н��� �ΰ���..(����/����Ʈ)
	this->_RenderAlphaTexture();

	return S_OK;
}

HRESULT ZTerrain::_RenderBaseTexture()
{
	m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
/*
	// 1�� �ε����� �̹��� �ؽ����� Į��� ���� OP�� ARG1, ARG2�� 1:1 ȥ���Ѵ�.
	m_pd3dDevice->SetTextureStageState(0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1);
  	m_pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	m_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1);
	m_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
/**/
	m_pd3dDevice->SetTextureStageState(0, D3DTSS_COLOROP,	D3DTOP_MODULATE);
	m_pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	m_pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
	m_pd3dDevice->SetTextureStageState(1, D3DTSS_COLOROP,   D3DTOP_MODULATE2X );	// MODULATE2�� ���´�.
	m_pd3dDevice->SetTextureStageState(1, D3DTSS_COLORARG1, D3DTA_TEXTURE );		// �ؽ�ó
	m_pd3dDevice->SetTextureStageState(1, D3DTSS_COLORARG2, D3DTA_CURRENT );		// �����

	m_pd3dDevice->SetTextureStageState( 2, D3DTSS_COLOROP,   D3DTOP_DISABLE );
	m_pd3dDevice->SetTextureStageState( 2, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );

	// �ؽ�ó ��ǥ�� ��Ʈ��
/**/
	m_pd3dDevice->SetTextureStageState( 0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT2 );
	m_pd3dDevice->SetTextureStageState( 0, D3DTSS_TEXCOORDINDEX, 0 );
	m_pd3dDevice->SetSamplerState( 0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP  );	// wap����..
	m_pd3dDevice->SetSamplerState( 0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP  );

	/**/
	m_pd3dDevice->SetTextureStageState( 1, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE );
	m_pd3dDevice->SetSamplerState( 1, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP );
	m_pd3dDevice->SetSamplerState( 1, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP );
	/**/

	D3DXMATRIXA16	matTemp;
	D3DXMatrixScaling( &matTemp, 512.f, 512.f, 1.f );
	m_pd3dDevice->SetTransform( D3DTS_TEXTURE0, &matTemp );
	m_pd3dDevice->SetTransform( D3DTS_TEXTURE1, &matTemp );
	/**/

	// Texture
	m_pd3dDevice->SetTexture(0,m_pTex[0]);
	m_pd3dDevice->SetTexture(1,m_pTex[1]);
	m_pd3dDevice->SetTextureStageState( 0, D3DTSS_TEXCOORDINDEX, 0 );
	m_pd3dDevice->SetTextureStageState( 1, D3DTSS_TEXCOORDINDEX, 0 );
	m_pd3dDevice->SetSamplerState(0, D3DSAMP_MAGFILTER,D3DTEXF_LINEAR);
	m_pd3dDevice->SetSamplerState(1, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );

	// Draw
	m_pd3dDevice->SetStreamSource(0,m_pVB,0, sizeof(TERRAINVERTEX));
	m_pd3dDevice->SetFVF(TERRAINVERTEX::FVF);
	m_pd3dDevice->SetIndices(m_pIB);
	m_pd3dDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST,0,0,m_cxDIB*m_czDIB, 0,m_nTriangles);

/*
	m_pd3dDevice->SetTexture(0,NULL);
	m_pd3dDevice->SetTexture(1,NULL);
	m_pd3dDevice->SetTexture(2,NULL);
	m_pd3dDevice->SetTextureStageState( 0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE );
	/**/
	return S_OK;
}

HRESULT ZTerrain::_RenderAlphaTexture()
{	
	if( m_plistTile.empty() )	return E_FAIL;
	
	// 0�� �ε������� ���� �ؽ��ĸ� ����
	// 1�� �ε������� �̹��� �ؽ��� ����
	
	// 0�� �ؽ��Ĵ� Į�� OP�� ���ؽ� Į�� ����ϰ�
	// ���� OP�� �ؽ��Ŀ��� �����´�	
	m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_SELECTARG1 );
	m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_DIFFUSE );
	m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1 );
	m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );

	// 0�� �ε��� ���� �ؽ��Ĵ� ��ü�� �ѹ��� �׷����� �ϹǷ�
	// Ÿ��ȭ ���� ���� ����
	m_pd3dDevice->SetTextureStageState( 0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE );
	m_pd3dDevice->SetSamplerState( 0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP );
	m_pd3dDevice->SetSamplerState( 0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP );
	
/* */
	m_pd3dDevice->SetTextureStageState(1, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT2 );
	m_pd3dDevice->SetTextureStageState(1, D3DTSS_TEXCOORDINDEX, 0 );
	m_pd3dDevice->SetSamplerState( 1, D3DSAMP_ADDRESSU,  D3DTADDRESS_WRAP );
	m_pd3dDevice->SetSamplerState( 1, D3DSAMP_ADDRESSV,  D3DTADDRESS_WRAP );

	D3DXMATRIXA16	matTemp;
	D3DXMatrixScaling( &matTemp, 256.f, 256.f, 1.f );
	m_pd3dDevice->SetTransform( D3DTS_TEXTURE0, &matTemp );
	m_pd3dDevice->SetTransform( D3DTS_TEXTURE1, &matTemp );

/**/
	// 1�� �ε����� �̹��� �ؽ����� Į��� ���� OP�� ARG1, ARG2�� 1:1 ȥ���Ѵ�.
	m_pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP, D3DTOP_MODULATE );
	m_pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP, D3DTOP_MODULATE );

	// ����Ű��
	m_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE  , TRUE);
	m_pd3dDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);	// �ҽ��÷���
	m_pd3dDevice->SetRenderState( D3DRS_DESTBLEND,D3DBLEND_INVSRCALPHA );

	vector<TILE*>::iterator iter = m_plistTile.begin();
	vector<TILE*>::iterator itEnd = m_plistTile.end();
	for( ; iter != itEnd; ++iter )
	{
		// [��Ƽ������] vector�� ���ͷ����ͷ� ������ �� �� ������.
		m_pd3dDevice->SetTexture( 0, (*iter)->pAlpha);		// AlphaTexture
		m_pd3dDevice->SetTexture( 1, (*iter)->pTex);		// ImageTexture
		
		m_pd3dDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, 0, m_cxDIB*m_czDIB, 0, m_nTriangles );
	}
		
	m_pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP, D3DTOP_DISABLE );
	m_pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP, D3DTOP_DISABLE );
	
	m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );

/**/
	//---------------------------------------------------------------------------
	// ����Ʈ�� �н�
	//---------------------------------------------------------------------------
	m_pd3dDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE4X);
	m_pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	
	m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
	m_pd3dDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_DESTCOLOR );
	m_pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ZERO );
	
	m_pd3dDevice->SetTexture( 0, m_pTex[1] );
	m_pd3dDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, 0, m_cxDIB*m_czDIB, 0, m_nTriangles );
	
	m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
/**/
	return S_OK;
}

/*
// ���� �߿��� ���� ������ �κ�
bool __fastcall ZTerrain::DrawAlphaTexture( int Index )
{
    float TexAlphaSize = S_WorldTile.TexAlphaSize;

    // ���ĸ��� �� �ȼ��� ���� ���� ������ ũ�⸦ ���Ѵ�.
    float PixSize  = (MAPSIZE * 2) / (float)TexAlphaSize;
    // �������� ���� �ִ� ũ�⿡ ������ ũ�⸦ ��������
    // ���ȼ��� ������ �ϴ��� ũ�⸦ ���Ѵ�.
    int   EditSize = (MainForm->MaxSize->Position / PixSize);

    // ��ŷ�� ��ǥ�� �̿��ؼ� ��ŷ�� ��ġ�� UV�� �˾Ƴ���.
    float PosU = GetPos.x / (float)(MAPSIZE * 2);
    float PosV = 1 - GetPos.z / (float)(MAPSIZE * 2);

    // ������ ���� UV�� �̿��ؼ� ���� ��ŷ�� ��ġ�� ���ĸ��� �ȼ� ��ġ�� ���Ѵ�.
    int PosX = TexAlphaSize * PosU;
    int PosY = TexAlphaSize * PosV;

    // ������ �� ���ĸ��� �̹����� ���� ��ġ�ȼ��� �� ��ġ �ȼ��� ���Ѵ�.
    int StartPosX = ( (PosX - EditSize) < 0 )? 0 : PosX - EditSize;
    int StartPosY = ( (PosY - EditSize) < 0 )? 0 : PosY - EditSize;
    int EndPosX = ( (PosX + EditSize) >= TexAlphaSize )? TexAlphaSize - 1: PosX + EditSize;
    int EndPosY = ( (PosY + EditSize) >= TexAlphaSize )? TexAlphaSize - 1: PosY + EditSize;

    MainForm->H_AppIsReady = false;

    D3DLOCKED_RECT      AlphaMap_Locked;
  	::ZeroMemory( &AlphaMap_Locked, sizeof(D3DLOCKED_RECT) );

    LPTileMap   _Tex = S_WorldTile.S_Tile[ Index ];
	if( FAILED ( _Tex->m_pAlpha->LockRect( 0, &AlphaMap_Locked, NULL, 0 ) ) )
        return false;

    unsigned char  Data;
    unsigned char  *DefBits;
	DefBits  = static_cast<unsigned char *>(AlphaMap_Locked.pBits);

    // ������ ���� ���ĸ��� ������ �ȼ��� ���鼭 ���� �����Ѵ�.
	for ( int Y = StartPosY; Y <= EndPosY; Y++ ) {
		for ( int X = StartPosX; X <= EndPosX; X++ ) {

            int in = ( AlphaMap_Locked.Pitch * Y ) + ( X * 4 );
            // �ش� �ȼ��� ���� �о�´�.
        	unsigned char Read = DefBits[ in ];

            // �������ϴ� �ȼ��� ��ġ�� 3D�� �����ϰ�, ��ŷ�� ��ġ�� �ȼ��� ��ġ��
            // 3D�� �����Ͽ� �ΰ��� �Ÿ��� ��� �Ѵ�.
            D3DXVECTOR3 Pix = D3DXVECTOR3( X * PixSize, 0.0f, ((TexAlphaSize) - Y) * PixSize ) -
                              D3DXVECTOR3( PosX * PixSize, 0.0f, ((TexAlphaSize) - PosY) * PixSize );
            float Len = D3DXVec3Length( &Pix );

            if ( Len <= MainForm->MinSize->Position ) {
                // �ּ��� ����� ���Ե� ���
                // ��� ���İ��� �ִ´�.
                Data = 0xff;
            } else if ( Len <= MainForm->MaxSize->Position ) {
                // �ִ����� ���Ե� �� �ִ�����������
                // ���������� ���İ��� ���Ѵ�.
               Len  -= MainForm->MinSize->Position;
               int Smooth = MainForm->MaxSize->Position - MainForm->MinSize->Position;
               Data = ( Smooth - Len ) / (float)Smooth  * 0xff;
            } else {
                // �ִ������� ������ ���� ���� ��� ���ĸ� �������� �ʴ´�.
                continue;
            }

            // �о�� �ȼ��� ���� ������ ���İ���
            // ����� �Ǵ� ���ϱ⿡ ���� ���� �����Ѵ�.
            if ( MainForm->EditAlpha->ItemIndex == 0 ) {
                Read = ( Read < Data )? Data : Read;
            } else {
                Read = ( (Read - Data ) < 0x00 )? 0x00 : (Read - Data );
            }

            // �����ؽ��Ŀ� ������ ���� ���İ��� �����Ѵ�.
            // �߿��Ѱ��� A�� �ٲٸ� �Ǹ�, RGB���� ���� �ٲ��� �ʾƵ� �ȴ�.
            // ���� ���İ��� Ȯ���� ���ؼ� ��� ������ ���� �־���Ҵ�.
            DefBits[ in++ ] = Read;
            DefBits[ in++ ] = Read;
            DefBits[ in++ ] = Read;
            DefBits[ in++ ] = Read;
		}
	}
  	if ( FAILED ( _Tex->m_pAlpha->UnlockRect(0) ) ) return false;

    MainForm->H_AppIsReady = true;

    return true;
}*/

float ZTerrain::GetHeight(int x,int z)			
{
	// �� ������ äũ, ������ ������ ������ 0�� ����
	if( 0 > x || 0 > z || x > m_cxDIB || z > m_czDIB ) return 0.0f;
	return this->GetVertex(x/m_vScale.x,z/m_vScale.z)->p.y; 
}

float ZTerrain::GetHeightEx( float x, float y )
{
	// ���� ������ �Ѿ�� ������ 0.0f
	if( x < 0.0f || x >= m_cxDIB || y < 0.0f || y >= m_czDIB )
		return 0.0f;

	// ���Ե� Ÿ���� 4���� ��ǥ�� ���Ѵ�.
	float fXLow   = (float) floor(x);
	float fXHigh  = (float) ceil(x);
	float fZLow   = (float) floor(y);
	float fZHigh  = (float) ceil(y);
	
	// �ش��ϴ� ĭ�� ���� 4���� ����
	//  *----*
	//  |  / |
	//  *----*
	float fPtHH = this->GetVertex( fXHigh, fZHigh )->p.y;
	float fPtHL = this->GetVertex( fXHigh, fZLow )->p.y; 
	float fPtLH = this->GetVertex( fXLow,  fZHigh)->p.y;
	float fPtLL = this->GetVertex( fXLow,  fZLow )->p.y;

	float fWeightX  = x - fXLow;	// x���� �Ÿ�
	float fWeightZ  = y - fZLow;	// y���� �Ÿ�

	// HH �ϰ� LH�� x������ �������� (Ÿ���� �Ʒ���)
	float fAvgZ_H   = fPtHH*fWeightX + fPtLH*(1.0f-fWeightX);       // avg( HH -> LH )
	// HL �ϰ� LL�� �������� (Ÿ���� ����)
	float fAvgZ_L   = fPtHL*fWeightX + fPtLL*(1.0f-fWeightX);       // avg( HL -> LL )
	// ������ ���� �ΰ��� ������ Z �������� ��������.
	float fAvg      = fAvgZ_H*fWeightZ + fAvgZ_L*(1.0f-fWeightZ);   // avg( Z_H -> Z_L )

	return fAvg;
	
/*
	int col0 = (int)x;s
	int row0 = (int)y;

	int col1 = col0 + 1;
	int row1 = row0 + 1;

	// ���� ������ �Ѿ�� ������ 0.0f
	if( x < 0.0f || x >= m_cxDIB || y < 0.0f || y >= m_czDIB )
		return 0.0f;

	//m_pHeightMap+x+z*m_cxDIB;
	
	// �ش��ϴ� ĭ�� ���� 4���� ����
	//  *----*
	//  |  / |
	//  *----*
	D3DXVECTOR3	v1 = this->GetVertex(col0,row0)->p;
	D3DXVECTOR3	v2 = this->GetVertex(col0,row1)->p;
	D3DXVECTOR3	v3 = this->GetVertex(col1,row0)->p;
	D3DXVECTOR3	v4 = this->GetVertex(col1,row1)->p;
	
	D3DXPLANE plane;

	//col1-x + row1-z) > 1.0f 

	// ������ ��ġ�� ����, �������� ��� �ﰢ���� ���ϴ��� �Ǻ��Ͽ� ����� ������ ����
	if( (x - col1 + y - row1) > 1.0f )
		D3DXPlaneFromPoints( &plane, &v2, &v3, &v4 );
	else
		D3DXPlaneFromPoints( &plane, &v1, &v2, &v3 );

	return (plane.a*x + plane.c*y + plane.d)/-plane.b;
	*/
}

HRESULT ZTerrain::AddSplattingTex(LPSTR lpAlphaTex, LPSTR lpImgTex)
{
	if(NULL == m_pd3dDevice) return E_FAIL;

	LPTILE pTile = new TILE;
	pTile->nIndex = m_plistTile.size();

	// AlphaTexture
	if( FAILED( D3DXCreateTextureFromFileEx(m_pd3dDevice,lpAlphaTex,D3DX_DEFAULT,D3DX_DEFAULT,0,0,
								D3DFMT_UNKNOWN,D3DPOOL_MANAGED,D3DX_DEFAULT,
								D3DX_DEFAULT,0,NULL,NULL, &pTile->pAlpha) ))
	{
		assert(false);
		return E_FAIL;
	}
	
	// ImageTexture
	if( FAILED( D3DXCreateTextureFromFileEx(m_pd3dDevice,lpImgTex,D3DX_DEFAULT,D3DX_DEFAULT,0,0,
								D3DFMT_UNKNOWN,D3DPOOL_MANAGED,D3DX_DEFAULT,
								D3DX_DEFAULT,0,NULL,NULL, &pTile->pTex) ))
	{
		assert(false);
		return E_FAIL;
	}

	m_plistTile.push_back(pTile);
	return S_OK;
}
