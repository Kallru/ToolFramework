// ZQuadTree.h

#ifndef _QUADTREE_
#define _QUADTREE_

struct TERRAINVERTEX
{
	enum _FVF { FVF=(D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_TEX1) };
	D3DXVECTOR3	p;
	D3DXVECTOR3	n;
	D3DXVECTOR2	t;
};

class ZFrustum;

class ZQuadTree
{
public:
	enum CornerType { CORNER_TL, CORNER_TR, CORNER_BL, CORNER_BR };

	// 쿼드와 절두체 관계
	enum QuadLocation { FRUSTUM_OUT = 0, FRUSTUM_PARTIALLY_IN, FRUSTUM_COMPLETELY_IN, FRUSTUM_UNKNOWN = -1 };

private:
	ZQuadTree * m_pChild[4];

	int m_nCenter;
	int m_nCorner[4];

	BOOL m_bChiled;		// 절두체에서 컬링된 노드인가
	float m_fRadius;	// 노드를 감싸는 경계구
	
private:
	ZQuadTree * _AddChiled(int nCornerTL, int nCornerTR, int nCornerBL, int nCornerBR);

	BOOL _SetCorners(int nCornerTL, int nCornerTR, int nCornerBL, int nCornerBR);
	BOOL _SubDivide();
	BOOL _IsVisible() { return (m_nCorner[CORNER_TR]-m_nCorner[CORNER_TL] <= 1); }
	int _GenTriIndex(int nTriangles, LPVOID pIndex);
	void _Destroy();

	// 현재 노드가 절두체에 포함인가
	int _IsInFrustum(TERRAINVERTEX* pHeightMap, ZFrustum* pFrustum);
	void _FrustumCull(TERRAINVERTEX* pHeightMap, ZFrustum* pFrustum);

public:

	// 최상위
	ZQuadTree(int cx, int cy);

	// 하위
	ZQuadTree(ZQuadTree* pParent);

	// 소멸자
	~ZQuadTree();

	// 쿼드트리를 구축
	BOOL Build(TERRAINVERTEX* pHeightMap);

	// 삼각형 인덱스를 만들고 출력할 개수를 반환
	int GenerateIndex(LPVOID pIndex, TERRAINVERTEX* pHeightMap, ZFrustum* pFrustum);
};

#endif