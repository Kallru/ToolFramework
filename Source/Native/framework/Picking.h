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

	// �� ���˿� �˸°� �����ε�
	HRESULT Create(const D3DXMATRIX& matWorld,size_t iSize);
	CPicking* Create(ZTerrain * pTerrain) { m_pTerrain=pTerrain; return this; }	// ������ŷ
	CPicking* Create(CXFile *pXFile) { m_pXFile = pXFile; return this; }

	BOOL Unprojection(LPDIRECT3DDEVICE9 pDevice, CCamera *pCamera, POINT *ptCursor,
						D3DXVECTOR3 *pvRayOrig=NULL, D3DXVECTOR3 *pvRayDir=NULL);

	// Create -> UnProjection -> ���Ŀ� ȣ���ؾ���
	DWORD TriangleIntersect(D3DXVECTOR3 *pvRayOrig, D3DXVECTOR3 *pvRayDir);

	// ���� ���� ����
	DWORD TriangleIntersectTerrain(D3DXVECTOR3 *pvRayOrig=NULL, D3DXVECTOR3 *pvRayDir=NULL);

private:
	// �ܺο��� ��������
	BOOL IntersectTriangle( const D3DXVECTOR3& , const D3DXVECTOR3& , D3DXVECTOR3&,
							D3DXVECTOR3& , D3DXVECTOR3& , FLOAT* , FLOAT* , FLOAT* );

public:
	D3DXVECTOR3 m_vIntersectPt;	// Ray�� ã�Ƴ� ����
	float		m_fDist;		// ���� ����� �Ÿ�
	D3DXVECTOR3 m_vTri[3];		// �ɸ� �ﰢ��

private:
	LPDIRECT3DVERTEXBUFFER9 m_pVertexBuffer;
	LPDIRECT3DINDEXBUFFER9  m_pIndexBuffer;

	// ����
	ZTerrain * m_pTerrain;

	// ��
	CXFile * m_pXFile;

	DWORD	m_dwMeshCnt;
	long	m_nFaceCount;
	D3DXMATRIX m_matWorld;

	D3DXVECTOR3 m_vRayDir;
	D3DXVECTOR3 m_vRayOrig;
};

#endif