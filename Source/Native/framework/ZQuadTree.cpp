// ZQuadTree.cpp

#include "stdafx.h"
#include "ZQuadTree.h"

// ���� ��Ʈ
ZQuadTree::ZQuadTree(int cx, int cy)
{
	int i;
	m_nCenter = 0;

	for(i=0; i<4; i++)
	{
		m_pChild[i] = NULL;
	}

	m_nCorner[CORNER_TL]	= 0;
	m_nCorner[CORNER_TR]	= cx - 1;
	m_nCorner[CORNER_BL]	= cx *(cy - 1);
	m_nCorner[CORNER_BR]	= cx * cy - 1;
	m_nCenter = (m_nCorner[CORNER_TL] + m_nCorner[CORNER_TR] + 
				 m_nCorner[CORNER_BL] + m_nCorner[CORNER_BR] ) / 4;

	m_fRadius = 0.0f;
	m_bChiled = FALSE;
}

// ���� ��� �ڽ� ����

ZQuadTree::ZQuadTree(ZQuadTree *pParent)
{
	m_nCenter = 0;

	int i;
	for(i=0; i<4; i++)
	{
		m_pChild[i] = NULL;
		m_nCorner[i] = 0;
	}

	m_bChiled = FALSE;
	m_fRadius = 0.0f;
}

ZQuadTree::~ZQuadTree()
{
	_Destroy();
}

// �޸𸮿��� ���带 ����
void ZQuadTree::_Destroy()
{
	// �ڽ� ��带 �Ҹ�
	for(int i=0; i<4; i++)
	{
		SAFE_DELETE(m_pChild[i]);
	}
}

// 4�� �ڳʰ��� ����
BOOL ZQuadTree::_SetCorners(int nCornerTL, int nCornerTR, int nCornerBL, int nCornerBR)
{
	m_nCorner[CORNER_TL] = nCornerTL;
	m_nCorner[CORNER_TR] = nCornerTR;
	m_nCorner[CORNER_BL] = nCornerBL;
	m_nCorner[CORNER_BR] = nCornerBR;

	m_nCenter = (m_nCorner[CORNER_TL] + m_nCorner[CORNER_TR] + 
				 m_nCorner[CORNER_BL] + m_nCorner[CORNER_BR] ) / 4;

	return TRUE;
}

// �ڽĳ�带 �߰�

ZQuadTree* ZQuadTree::_AddChiled(int nCornerTL,int nCornerTR, int nCornerBL, int nCornerBR)
{
	ZQuadTree * pChild;

	pChild = new ZQuadTree(this);
	pChild->_SetCorners(nCornerTL,nCornerTR, nCornerBL, nCornerBR);

	return pChild;
}

// ���带 4���� �ڽ� Ʈ���� ���� ����
BOOL ZQuadTree::_SubDivide()
{
	int nTopCenter;
	int nBottomCenter;
	int nLeftCenter;
	int nRightCenter;
	int nCentralPoint;

	// ��ܺ� ���
	nTopCenter		= (m_nCorner[CORNER_TL] + m_nCorner[CORNER_TR]) / 2;
	// �ϴܺ� ���
	nBottomCenter	= (m_nCorner[CORNER_BL] + m_nCorner[CORNER_BR]) / 2;
	// ������	
	nLeftCenter		= (m_nCorner[CORNER_TL] + m_nCorner[CORNER_BL]) / 2;
	// ������
	nRightCenter	= (m_nCorner[CORNER_TR] + m_nCorner[CORNER_BR]) / 2;
	// �Ѱ��
	nCentralPoint	= ( m_nCorner[CORNER_TL] + m_nCorner[CORNER_TR] + 
						m_nCorner[CORNER_BL] + m_nCorner[CORNER_BR] ) / 4;

	// ���̻� ������ �Ұ����Ѱ�
	if( (m_nCorner[CORNER_TR]-m_nCorner[CORNER_TL]) <= 1 )
	{
		return false;
	}

	// 4���� �ڽ� ��� �߰�
	m_pChild[CORNER_TL] = _AddChiled(m_nCorner[CORNER_TL],nTopCenter, nLeftCenter, nCentralPoint);
	m_pChild[CORNER_TR] = _AddChiled(nTopCenter, m_nCorner[CORNER_TR], nCentralPoint, nRightCenter);
	m_pChild[CORNER_BL] = _AddChiled(nLeftCenter, nCentralPoint, m_nCorner[CORNER_BL],nBottomCenter);
	m_pChild[CORNER_BR] = _AddChiled(nCentralPoint,nRightCenter,nBottomCenter,m_nCorner[CORNER_BR]);

	return true;
}

int ZQuadTree::_GenTriIndex(int nTris, LPVOID pIndex)
{
	// �ø��� ����� �� ����
	if(m_bChiled)
	{
		m_bChiled= FALSE;
		return nTris;
	}

	// ���� ��尡 ��µǾ�� �ϴ°�
	if( this->_IsVisible() )
	{
//#ifdef _USE_INDEX16
		LPWORD p = ((LPWORD)pIndex) + nTris * 3;
//#else
//		LPWORD p = ((LPDWORD)pIndex) + nTris * 3;
//#endif
		*p++ = m_nCorner[0];
		*p++ = m_nCorner[1];
		*p++ = m_nCorner[2];
		nTris++;
		*p++ = m_nCorner[2];
		*p++ = m_nCorner[1];
		*p++ = m_nCorner[3];
		nTris++;

		return nTris;
	}

	// �ڽ� ��� �˻�
	if(m_pChild[CORNER_TL]) nTris = m_pChild[CORNER_TL]->_GenTriIndex(nTris,pIndex);
	if(m_pChild[CORNER_TR]) nTris = m_pChild[CORNER_TR]->_GenTriIndex(nTris,pIndex);
	if(m_pChild[CORNER_BL]) nTris = m_pChild[CORNER_BL]->_GenTriIndex(nTris,pIndex);
	if(m_pChild[CORNER_BR]) nTris = m_pChild[CORNER_BR]->_GenTriIndex(nTris,pIndex);

	return nTris;
}

int ZQuadTree::_IsInFrustum(TERRAINVERTEX* pHeightMap, ZFrustum* pFrustum)
{
	BOOL b[4];
	BOOL bInSphere;

	// ��豸 �ȿ� �ִ°�
	bInSphere = pFrustum->IsInSphere((D3DXVECTOR3*)(pHeightMap+m_nCenter), m_fRadius);
	if(!bInSphere) return FRUSTUM_OUT;

	b[0] = pFrustum->IsIn((D3DXVECTOR3*)(pHeightMap+m_nCorner[0]));
	b[1] = pFrustum->IsIn((D3DXVECTOR3*)(pHeightMap+m_nCorner[1]));
	b[2] = pFrustum->IsIn((D3DXVECTOR3*)(pHeightMap+m_nCorner[2]));
	b[3] = pFrustum->IsIn((D3DXVECTOR3*)(pHeightMap+m_nCorner[3]));

	// 4�� ��� ����ü �ȿ� ����
	if((b[0]+b[1]+b[2]+b[3])==4)  return FRUSTUM_COMPLETELY_IN;

	return FRUSTUM_PARTIALLY_IN;
}

// IsinFrustum�� ���� �ø� ����
void ZQuadTree::_FrustumCull(TERRAINVERTEX* pHeightMap, ZFrustum* pFrustum)
{
	int ret;
	ret = this->_IsInFrustum(pHeightMap,pFrustum);
	
	switch(ret)
	{
	case FRUSTUM_COMPLETELY_IN:	// ����ü�� ���� ����
		m_bChiled = FALSE;	
		return ; // ���� ��� �˻� �ʿ� ����

	case FRUSTUM_PARTIALLY_IN:
		m_bChiled = FALSE;
		break;	// ���� ��� �˻� �ʿ�

	case FRUSTUM_OUT:
		m_bChiled = TRUE;
		return;
	}

	if(m_pChild[0]) m_pChild[0]->_FrustumCull(pHeightMap,pFrustum);
	if(m_pChild[1]) m_pChild[1]->_FrustumCull(pHeightMap,pFrustum);
	if(m_pChild[2]) m_pChild[2]->_FrustumCull(pHeightMap,pFrustum);
	if(m_pChild[3]) m_pChild[3]->_FrustumCull(pHeightMap,pFrustum);
}

BOOL ZQuadTree::Build(TERRAINVERTEX* pHeightMap)
{
	if(this->_SubDivide())
	{
		// ���� ��ܰ� ���� �ϴ��� �Ÿ����
		D3DXVECTOR3 v = *((D3DXVECTOR3 *)(pHeightMap+m_nCorner[CORNER_TL])) - 
						*((D3DXVECTOR3 *)(pHeightMap+m_nCorner[CORNER_BR]));

		// V�� �Ÿ����� �̳�带 ���δ� ��豸�� �����̸���,
		// 2�� ������ �������� ���Ѵ�.
		m_fRadius	= D3DXVec3Length(&v) / 2.0f;
		m_pChild[CORNER_TL]->Build(pHeightMap);
		m_pChild[CORNER_TR]->Build(pHeightMap);
		m_pChild[CORNER_BL]->Build(pHeightMap);
		m_pChild[CORNER_BR]->Build(pHeightMap);
	}

	return TRUE;
}

// �ﰢ�� �ε����� ����� ����� ������ ����
int ZQuadTree::GenerateIndex(LPVOID pIndex, TERRAINVERTEX* pHeightMap, ZFrustum* pFrustum)
{
	this->_FrustumCull(pHeightMap,pFrustum);
	return this->_GenTriIndex(0, pIndex);
}