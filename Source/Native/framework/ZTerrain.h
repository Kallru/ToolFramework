// ZTerrain.h

#ifndef _TERRAIN_
#define _TERRAIN_

#define MAX_TERRAIN_TEX	4

struct TRIINDEX
{
	WORD _0, _1, _2;
};

typedef struct TILE
{
	int					nIndex;
	LPDIRECT3DTEXTURE9	pAlpha;
	LPDIRECT3DTEXTURE9	pTex;

	TILE():nIndex(-1),pAlpha(NULL),pTex(NULL) {}
	~TILE()
	{
		nIndex=-1;
		pAlpha->Release();
		pTex->Release();
	}
}TILE, *LPTILE;

class ZTerrain
{
private:
	int		m_cxDIB;
	int		m_czDIB;
	int		m_cyDIB;

	D3DXVECTOR3				m_vScale;
	TERRAINVERTEX*			m_pHeightMap;
	LPDIRECT3DDEVICE9		m_pd3dDevice;
	LPDIRECT3DTEXTURE9		m_pTex[MAX_TERRAIN_TEX];
	LPDIRECT3DVERTEXBUFFER9 m_pVB;
	LPDIRECT3DINDEXBUFFER9	m_pIB;

	int						m_nTriangles;
	ZQuadTree*				m_pQuadTree;

	// 피킹에 쓰일 데이터들
	int						m_nCullFaceNum;	// 개수
	TRIINDEX *				m_pFace;		// 인덱스 정보

	// 스플래팅에 쓰이는 타일(알파/이미지텍)
	vector<TILE*>			m_plistTile;

public:
	ZTerrain();
	~ZTerrain();

	HRESULT Create(LPDIRECT3DDEVICE9 pDe, D3DXVECTOR3 *pScale, LPSTR lpBMPFileName, LPSTR lpTexFile[MAX_TERRAIN_TEX]);

private:
	HRESULT _Destroy();
	HRESULT	_LoadTexture(LPSTR lpTexFileName[MAX_TERRAIN_TEX]);
	HRESULT _BuildHeightMap(LPSTR lpFileName);
	HRESULT _BuildQuadTree();
	HRESULT _CreateVIB();

	HRESULT _RenderAlphaTexture();
	HRESULT _RenderBaseTexture();

public:
	TERRAINVERTEX*	GetVertex(int x, int z) { return m_pHeightMap+x+z*m_cxDIB; }
	float GetHeight(int x,int z);
	float GetHeightEx( float x, float y );
	int GetCXDIB()	{ return m_cxDIB; }
	int GetCZDIB()	{ return m_czDIB; }
	float GetCXTerrain()	{ return m_cxDIB * m_vScale.x; }
	float GetCYTerrain()	{ return m_cyDIB * m_vScale.y; }
	float GetCZTerrain()	{ return m_czDIB * m_vScale.z; }

	void GetIndexBuffer(LPDIRECT3DINDEXBUFFER9	*ppIB) { *ppIB = m_pIB; }

	int GetTriangles()	{ return m_nTriangles; }
	int GetCullTri()	{ return m_nCullFaceNum; }
	
	TRIINDEX * GetCullIndex() { return m_pFace; }
	
	HRESULT IndexLock(LPDWORD *ppI) { return m_pIB->Lock(0, (m_cxDIB-1)*(m_czDIB-1)*2*sizeof(TRIINDEX), (void**)ppI,0 ); }
	HRESULT IndexUnlock()		  { return m_pIB->Unlock(); }

	HRESULT Render( ZFrustum* pFrustum );

	HRESULT AddSplattingTex(LPSTR lpAlphaTex, LPSTR lpImgTex);	// 어쩌면 나중에 없어질 함수일지도..
	//HRESULT AddAlphaTexture(LPSTR lpFileName);
};

#endif