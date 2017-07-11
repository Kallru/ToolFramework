//------------------------------------------------------------------------------
// File. Picking.h
//------------------------------------------------------------------------------

#ifndef __PICKING__
#define __PICKING__

struct PICKVERTEX
{
    D3DXVECTOR3 vV;
	D3DXVECTOR3 vN;
};

class ZTerrain;

class CPicking
{
public:
	CPicking();
	~CPicking();

	void Init();

	// 각 포맷에 알맞게 오버로딩
	HRESULT Create(const D3DXMATRIX& matWorld,size_t iSize);
	CPicking* Create(ZTerrain * pTerrain) { m_pTerrain=pTerrain; return this; }	// 지형피킹
	CPicking* Create(CXFile *pXFile) { m_pXFile = pXFile; return this; }

	BOOL Unprojection(LPDIRECT3DDEVICE9 pDevice, CCamera *pCamera, POINT *ptCursor,
						D3DXVECTOR3 *pvRayOrig=NULL, D3DXVECTOR3 *pvRayDir=NULL);

	// Create -> UnProjection -> 이후에 호출해야함
	DWORD TriangleIntersect(D3DXVECTOR3 *pvRayOrig, D3DXVECTOR3 *pvRayDir);

	// 레이 직접 지정
	DWORD TriangleIntersectTerrain(D3DXVECTOR3 *pvRayOrig=NULL, D3DXVECTOR3 *pvRayDir=NULL);

private:
	// 외부에선 쓰지마삼
	BOOL IntersectTriangle( const D3DXVECTOR3& , const D3DXVECTOR3& , D3DXVECTOR3&,
							D3DXVECTOR3& , D3DXVECTOR3& , FLOAT* , FLOAT* , FLOAT* );

public:
	D3DXVECTOR3 m_vIntersectPt;	// Ray가 찾아낸 교점
	float		m_fDist;		// 가장 가까운 거리
	D3DXVECTOR3 m_vTri[3];		// 걸린 삼각형

private:
	LPDIRECT3DVERTEXBUFFER9 m_pVertexBuffer;
	LPDIRECT3DINDEXBUFFER9  m_pIndexBuffer;

	// 지형
	ZTerrain * m_pTerrain;

	// 모델
	CXFile * m_pXFile;

	DWORD	m_dwMeshCnt;
	long	m_nFaceCount;
	D3DXMATRIX m_matWorld;

	D3DXVECTOR3 m_vRayDir;
	D3DXVECTOR3 m_vRayOrig;
};

#endif