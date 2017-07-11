// ZQuadTree.cpp

#include "stdafx.h"
#include "ZQuadTree.h"

// 최초 루트
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

// 하위 노드 자식 생성

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

// 메모리에서 쿼드를 삭제
void ZQuadTree::_Destroy()
{
	// 자식 노드를 소멸
	for(int i=0; i<4; i++)
	{
		SAFE_DELETE(m_pChild[i]);
	}
}

// 4개 코너값을 셋팅
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

// 자식노드를 추가

ZQuadTree* ZQuadTree::_AddChiled(int nCornerTL,int nCornerTR, int nCornerBL, int nCornerBR)
{
	ZQuadTree * pChild;

	pChild = new ZQuadTree(this);
	pChild->_SetCorners(nCornerTL,nCornerTR, nCornerBL, nCornerBR);

	return pChild;
}

// 쿼드를 4개의 자식 트리로 하위 분할
BOOL ZQuadTree::_SubDivide()
{
	int nTopCenter;
	int nBottomCenter;
	int nLeftCenter;
	int nRightCenter;
	int nCentralPoint;

	// 상단변 가운데
	nTopCenter		= (m_nCorner[CORNER_TL] + m_nCorner[CORNER_TR]) / 2;
	// 하단변 가운데
	nBottomCenter	= (m_nCorner[CORNER_BL] + m_nCorner[CORNER_BR]) / 2;
	// 좌측변	
	nLeftCenter		= (m_nCorner[CORNER_TL] + m_nCorner[CORNER_BL]) / 2;
	// 오측변
	nRightCenter	= (m_nCorner[CORNER_TR] + m_nCorner[CORNER_BR]) / 2;
	// 한가운데
	nCentralPoint	= ( m_nCorner[CORNER_TL] + m_nCorner[CORNER_TR] + 
						m_nCorner[CORNER_BL] + m_nCorner[CORNER_BR] ) / 4;

	// 더이상 분할이 불가능한가
	if( (m_nCorner[CORNER_TR]-m_nCorner[CORNER_TL]) <= 1 )
	{
		return false;
	}

	// 4개의 자식 노드 추가
	m_pChild[CORNER_TL] = _AddChiled(m_nCorner[CORNER_TL],nTopCenter, nLeftCenter, nCentralPoint);
	m_pChild[CORNER_TR] = _AddChiled(nTopCenter, m_nCorner[CORNER_TR], nCentralPoint, nRightCenter);
	m_pChild[CORNER_BL] = _AddChiled(nLeftCenter, nCentralPoint, m_nCorner[CORNER_BL],nBottomCenter);
	m_pChild[CORNER_BR] = _AddChiled(nCentralPoint,nRightCenter,nBottomCenter,m_nCorner[CORNER_BR]);

	return true;
}

int ZQuadTree::_GenTriIndex(int nTris, LPVOID pIndex)
{
	// 컬링된 노드라면 걍 리턴
	if(m_bChiled)
	{
		m_bChiled= FALSE;
		return nTris;
	}

	// 현재 노드가 출력되어야 하는가
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

	// 자식 노드 검색
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

	// 경계구 안에 있는가
	bInSphere = pFrustum->IsInSphere((D3DXVECTOR3*)(pHeightMap+m_nCenter), m_fRadius);
	if(!bInSphere) return FRUSTUM_OUT;

	b[0] = pFrustum->IsIn((D3DXVECTOR3*)(pHeightMap+m_nCorner[0]));
	b[1] = pFrustum->IsIn((D3DXVECTOR3*)(pHeightMap+m_nCorner[1]));
	b[2] = pFrustum->IsIn((D3DXVECTOR3*)(pHeightMap+m_nCorner[2]));
	b[3] = pFrustum->IsIn((D3DXVECTOR3*)(pHeightMap+m_nCorner[3]));

	// 4개 모두 절두체 안에 있음
	if((b[0]+b[1]+b[2]+b[3])==4)  return FRUSTUM_COMPLETELY_IN;

	return FRUSTUM_PARTIALLY_IN;
}

// IsinFrustum에 따라 컬링 수행
void ZQuadTree::_FrustumCull(TERRAINVERTEX* pHeightMap, ZFrustum* pFrustum)
{
	int ret;
	ret = this->_IsInFrustum(pHeightMap,pFrustum);
	
	switch(ret)
	{
	case FRUSTUM_COMPLETELY_IN:	// 절두체에 완전 포함
		m_bChiled = FALSE;	
		return ; // 하위 노드 검색 필요 없음

	case FRUSTUM_PARTIALLY_IN:
		m_bChiled = FALSE;
		break;	// 하위 노드 검색 필요

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
		// 좌측 상단과 우측 하단의 거리계산
		D3DXVECTOR3 v = *((D3DXVECTOR3 *)(pHeightMap+m_nCorner[CORNER_TL])) - 
						*((D3DXVECTOR3 *)(pHeightMap+m_nCorner[CORNER_BR]));

		// V의 거리값이 이노드를 감싸는 경계구의 지름이르모,
		// 2로 나누어 반지름을 구한다.
		m_fRadius	= D3DXVec3Length(&v) / 2.0f;
		m_pChild[CORNER_TL]->Build(pHeightMap);
		m_pChild[CORNER_TR]->Build(pHeightMap);
		m_pChild[CORNER_BL]->Build(pHeightMap);
		m_pChild[CORNER_BR]->Build(pHeightMap);
	}

	return TRUE;
}

// 삼각형 인덱스를 만들고 출력할 개수를 리턴
int ZQuadTree::GenerateIndex(LPVOID pIndex, TERRAINVERTEX* pHeightMap, ZFrustum* pFrustum)
{
	this->_FrustumCull(pHeightMap,pFrustum);
	return this->_GenTriIndex(0, pIndex);
}