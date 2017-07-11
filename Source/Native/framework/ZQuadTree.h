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

	// ����� ����ü ����
	enum QuadLocation { FRUSTUM_OUT = 0, FRUSTUM_PARTIALLY_IN, FRUSTUM_COMPLETELY_IN, FRUSTUM_UNKNOWN = -1 };

private:
	ZQuadTree * m_pChild[4];

	int m_nCenter;
	int m_nCorner[4];

	BOOL m_bChiled;		// ����ü���� �ø��� ����ΰ�
	float m_fRadius;	// ��带 ���δ� ��豸
	
private:
	ZQuadTree * _AddChiled(int nCornerTL, int nCornerTR, int nCornerBL, int nCornerBR);

	BOOL _SetCorners(int nCornerTL, int nCornerTR, int nCornerBL, int nCornerBR);
	BOOL _SubDivide();
	BOOL _IsVisible() { return (m_nCorner[CORNER_TR]-m_nCorner[CORNER_TL] <= 1); }
	int _GenTriIndex(int nTriangles, LPVOID pIndex);
	void _Destroy();

	// ���� ��尡 ����ü�� �����ΰ�
	int _IsInFrustum(TERRAINVERTEX* pHeightMap, ZFrustum* pFrustum);
	void _FrustumCull(TERRAINVERTEX* pHeightMap, ZFrustum* pFrustum);

public:

	// �ֻ���
	ZQuadTree(int cx, int cy);

	// ����
	ZQuadTree(ZQuadTree* pParent);

	// �Ҹ���
	~ZQuadTree();

	// ����Ʈ���� ����
	BOOL Build(TERRAINVERTEX* pHeightMap);

	// �ﰢ�� �ε����� ����� ����� ������ ��ȯ
	int GenerateIndex(LPVOID pIndex, TERRAINVERTEX* pHeightMap, ZFrustum* pFrustum);
};

#endif