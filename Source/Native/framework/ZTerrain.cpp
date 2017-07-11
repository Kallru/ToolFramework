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

	// 높이맵 생성
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

	// 알파 텍스처 셋팅 - 현재 임시
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
			// 0번 텍스처가 아니라면, 실패해도 에러를 내지 않는다.
			// 0번텍스처는 가장 기본 베이스 텍스처로, 읽지 못한다면 에러를 내는게 좋겠다.
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
	// 높이맵  "map128.bmp"
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

	DWORD dwSize = (m_cxDIB-1)*(m_czDIB-1)* 2;	// 왜 2를 곱하는걸까

	if(FAILED(m_pd3dDevice->CreateIndexBuffer( dwSize * sizeof(TRIINDEX),0, D3DFMT_INDEX16,
		D3DPOOL_DEFAULT, &m_pIB, NULL )))
	{
		this->_Destroy();
		return E_FAIL;
	}

	// 버퍼가 너무 큰데 ㅡㅡ;
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

	// 피킹에 쓰일 인덱스를 만듬
	DWORD dwSize = (m_cxDIB-1)*(m_czDIB-1)* 2;
	ZeroMemory(m_pFace,dwSize * sizeof(TRIINDEX));
	m_nCullFaceNum = m_pQuadTree->GenerateIndex(m_pFace, m_pHeightMap, &pFrustum[1]);	// 프러스텀1번에 약간 문제가 있는듯

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
	//  - 맵 랜더 설명 -
	//  총 3개의 멀티 패스로 이루어져 있다. ㅠ_ㅠ (원패스로 줄이고 싶다;;;)
	//	첫번째 패스 - 사용텍스처는 2개로 베이스 텍스처/라이트텍스처로 이루어져있다.	(함수 : _RenderBaseTexture)
	//	두번째 패스 - 스플래팅을 하는 패스, 알파맵/해당 이미지텍스처로 이루어져있다.(함수 : _RenderAlphaTexture)
	//	세번째 패스 - 스플래팅이 끝난 바로 직후 이루어지며 라이트 텍스처를 최종적으로 입힌다.(두번째 패스 바로 밑에서 함)
	//	- 라이트맵을 마지막 패스에 한번 더 하는건, 스플래팅한 부분에 라이트맵을 적용하기 위함..
	//---------------------------------------------------------------------------------------------------------------------
	
	if(FAILED( this->_RenderBaseTexture() ))
	{
		assert(false);
		return E_FAIL;
	}

	// 알파 랜더는 Fail 될수 있다. (알파맵을 안쓸수도 있으니까)
	// 이 함수에서 패스를 두개씀..(스플/라이트)
	this->_RenderAlphaTexture();

	return S_OK;
}

HRESULT ZTerrain::_RenderBaseTexture()
{
	m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
/*
	// 1번 인덱스의 이미지 텍스쳐의 칼라와 알파 OP를 ARG1, ARG2를 1:1 혼합한다.
	m_pd3dDevice->SetTextureStageState(0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1);
  	m_pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	m_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1);
	m_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
/**/
	m_pd3dDevice->SetTextureStageState(0, D3DTSS_COLOROP,	D3DTOP_MODULATE);
	m_pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	m_pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
	m_pd3dDevice->SetTextureStageState(1, D3DTSS_COLOROP,   D3DTOP_MODULATE2X );	// MODULATE2로 섞는다.
	m_pd3dDevice->SetTextureStageState(1, D3DTSS_COLORARG1, D3DTA_TEXTURE );		// 텍스처
	m_pd3dDevice->SetTextureStageState(1, D3DTSS_COLORARG2, D3DTA_CURRENT );		// 현재색

	m_pd3dDevice->SetTextureStageState( 2, D3DTSS_COLOROP,   D3DTOP_DISABLE );
	m_pd3dDevice->SetTextureStageState( 2, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );

	// 텍스처 좌표를 컨트롤
/**/
	m_pd3dDevice->SetTextureStageState( 0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT2 );
	m_pd3dDevice->SetTextureStageState( 0, D3DTSS_TEXCOORDINDEX, 0 );
	m_pd3dDevice->SetSamplerState( 0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP  );	// wap으로..
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
	
	// 0번 인덱스에는 알파 텍스쳐를 셋팅
	// 1번 인덱스에는 이미지 텍스쳐 셋팅
	
	// 0번 텍스쳐는 칼라 OP를 버텍스 칼라만 사용하고
	// 알파 OP만 텍스쳐에서 가져온다	
	m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_SELECTARG1 );
	m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_DIFFUSE );
	m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1 );
	m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );

	// 0번 인덱스 알파 텍스쳐는 전체에 한번만 그려져야 하므로
	// 타일화 셋팅 하지 않음
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
	// 1번 인덱스의 이미지 텍스쳐의 칼라와 알파 OP를 ARG1, ARG2를 1:1 혼합한다.
	m_pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP, D3DTOP_MODULATE );
	m_pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP, D3DTOP_MODULATE );

	// 알파키고
	m_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE  , TRUE);
	m_pd3dDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);	// 소스컬러로
	m_pd3dDevice->SetRenderState( D3DRS_DESTBLEND,D3DBLEND_INVSRCALPHA );

	vector<TILE*>::iterator iter = m_plistTile.begin();
	vector<TILE*>::iterator itEnd = m_plistTile.end();
	for( ; iter != itEnd; ++iter )
	{
		// [옵티마이즈] vector을 이터레이터로 돌리면 좀 더 빠르다.
		m_pd3dDevice->SetTexture( 0, (*iter)->pAlpha);		// AlphaTexture
		m_pd3dDevice->SetTexture( 1, (*iter)->pTex);		// ImageTexture
		
		m_pd3dDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, 0, m_cxDIB*m_czDIB, 0, m_nTriangles );
	}
		
	m_pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP, D3DTOP_DISABLE );
	m_pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP, D3DTOP_DISABLE );
	
	m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );

/**/
	//---------------------------------------------------------------------------
	// 라이트맵 패스
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
// 가장 중요한 알파 에디터 부분
bool __fastcall ZTerrain::DrawAlphaTexture( int Index )
{
    float TexAlphaSize = S_WorldTile.TexAlphaSize;

    // 알파맵의 한 픽셀이 월드 상의 한점의 크기를 구한다.
    float PixSize  = (MAPSIZE * 2) / (float)TexAlphaSize;
    // 에디터의 원의 최대 크기에 한점의 크기를 나누워서
    // 몇픽셀을 에디터 하는지 크기를 구한다.
    int   EditSize = (MainForm->MaxSize->Position / PixSize);

    // 피킹한 좌표를 이용해서 피킹한 위치의 UV를 알아낸다.
    float PosU = GetPos.x / (float)(MAPSIZE * 2);
    float PosV = 1 - GetPos.z / (float)(MAPSIZE * 2);

    // 위에서 구한 UV를 이용해서 지금 피킹된 위치의 알파맵의 픽셀 위치를 구한다.
    int PosX = TexAlphaSize * PosU;
    int PosY = TexAlphaSize * PosV;

    // 에디터 할 알파맵의 이미지의 시작 위치픽셀과 끝 위치 픽셀을 구한다.
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

    // 위에서 구한 알파맵의 수정할 픽셀을 돌면서 값을 셋팅한다.
	for ( int Y = StartPosY; Y <= EndPosY; Y++ ) {
		for ( int X = StartPosX; X <= EndPosX; X++ ) {

            int in = ( AlphaMap_Locked.Pitch * Y ) + ( X * 4 );
            // 해당 픽셀의 값을 읽어온다.
        	unsigned char Read = DefBits[ in ];

            // 에디터하는 픽셀의 위치를 3D로 번경하고, 피킹된 위치의 픽셀의 위치를
            // 3D로 변경하여 두개의 거리를 계산 한다.
            D3DXVECTOR3 Pix = D3DXVECTOR3( X * PixSize, 0.0f, ((TexAlphaSize) - Y) * PixSize ) -
                              D3DXVECTOR3( PosX * PixSize, 0.0f, ((TexAlphaSize) - PosY) * PixSize );
            float Len = D3DXVec3Length( &Pix );

            if ( Len <= MainForm->MinSize->Position ) {
                // 최소의 사이즈에 포함될 경우
                // 흰색 알파값을 넣는다.
                Data = 0xff;
            } else if ( Len <= MainForm->MaxSize->Position ) {
                // 최대사이즈에 포함될 우 최대사이즈까지의
                // 선영보간한 알파값을 구한다.
               Len  -= MainForm->MinSize->Position;
               int Smooth = MainForm->MaxSize->Position - MainForm->MinSize->Position;
               Data = ( Smooth - Len ) / (float)Smooth  * 0xff;
            } else {
                // 최대사이즈에도 포함이 되지 않을 경우 알파를 셋팅하지 않는다.
                continue;
            }

            // 읽어온 픽셀에 새로 생성된 알파값을
            // 지우기 또는 더하기에 따라 새로 셋팅한다.
            if ( MainForm->EditAlpha->ItemIndex == 0 ) {
                Read = ( Read < Data )? Data : Read;
            } else {
                Read = ( (Read - Data ) < 0x00 )? 0x00 : (Read - Data );
            }

            // 알파텍스쳐에 위에서 구한 알파값을 갱신한다.
            // 중요한것은 A만 바꾸며 되며, RGB값은 구지 바꾸지 않아도 된다.
            // 물론 알파값의 확인을 위해서 모두 동일한 값을 넣어놓았다.
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
	// 맵 사이즈 채크, 밖으로 나가면 무조건 0을 리턴
	if( 0 > x || 0 > z || x > m_cxDIB || z > m_czDIB ) return 0.0f;
	return this->GetVertex(x/m_vScale.x,z/m_vScale.z)->p.y; 
}

float ZTerrain::GetHeightEx( float x, float y )
{
	// 맵의 범위를 넘어서면 무조건 0.0f
	if( x < 0.0f || x >= m_cxDIB || y < 0.0f || y >= m_czDIB )
		return 0.0f;

	// 포함된 타일의 4군데 좌표를 구한다.
	float fXLow   = (float) floor(x);
	float fXHigh  = (float) ceil(x);
	float fZLow   = (float) floor(y);
	float fZHigh  = (float) ceil(y);
	
	// 해당하는 칸의 정점 4개를 얻어옴
	//  *----*
	//  |  / |
	//  *----*
	float fPtHH = this->GetVertex( fXHigh, fZHigh )->p.y;
	float fPtHL = this->GetVertex( fXHigh, fZLow )->p.y; 
	float fPtLH = this->GetVertex( fXLow,  fZHigh)->p.y;
	float fPtLL = this->GetVertex( fXLow,  fZLow )->p.y;

	float fWeightX  = x - fXLow;	// x축의 거리
	float fWeightZ  = y - fZLow;	// y축의 거리

	// HH 하고 LH의 x축으로 선형보간 (타일의 아랫변)
	float fAvgZ_H   = fPtHH*fWeightX + fPtLH*(1.0f-fWeightX);       // avg( HH -> LH )
	// HL 하고 LL의 선형보간 (타일의 윗변)
	float fAvgZ_L   = fPtHL*fWeightX + fPtLL*(1.0f-fWeightX);       // avg( HL -> LL )
	// 위에서 구한 두개를 가지고 Z 기준으로 선형보간.
	float fAvg      = fAvgZ_H*fWeightZ + fAvgZ_L*(1.0f-fWeightZ);   // avg( Z_H -> Z_L )

	return fAvg;
	
/*
	int col0 = (int)x;s
	int row0 = (int)y;

	int col1 = col0 + 1;
	int row1 = row0 + 1;

	// 맵의 범위를 넘어서면 무조건 0.0f
	if( x < 0.0f || x >= m_cxDIB || y < 0.0f || y >= m_czDIB )
		return 0.0f;

	//m_pHeightMap+x+z*m_cxDIB;
	
	// 해당하는 칸의 정점 4개를 얻어옴
	//  *----*
	//  |  / |
	//  *----*
	D3DXVECTOR3	v1 = this->GetVertex(col0,row0)->p;
	D3DXVECTOR3	v2 = this->GetVertex(col0,row1)->p;
	D3DXVECTOR3	v3 = this->GetVertex(col1,row0)->p;
	D3DXVECTOR3	v4 = this->GetVertex(col1,row1)->p;
	
	D3DXPLANE plane;

	//col1-x + row1-z) > 1.0f 

	// 정점의 위치가 왼쪽, 오른쪽중 어느 삼각형에 속하는지 판별하여 평면의 방정식 대입
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
