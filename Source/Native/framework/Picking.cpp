//------------------------------------------------------------------------------
// File. Picking.cpp
//------------------------------------------------------------------------------

#include "StdAfx.h"
#include "Picking.h"

CPicking::CPicking()
{
	this->Init();
}

CPicking::~CPicking()
{}

void CPicking::Init()
{
	m_pIndexBuffer	= NULL;
	m_pVertexBuffer = NULL;
	m_pTerrain		= NULL;
	D3DXMatrixIdentity(&m_matWorld);

	m_fDist			= 1000.f;
	m_dwMeshCnt		= 0;
	m_nFaceCount	= 0;

	m_vRayDir		= D3DXVECTOR3(0,0,0);
	m_vRayOrig		= D3DXVECTOR3(0,0,0);
	m_vIntersectPt	= D3DXVECTOR3(0,0,0);
	m_vTri[0]		= D3DXVECTOR3(0,0,0);
	m_vTri[1]		= D3DXVECTOR3(0,0,0);
	m_vTri[2]		= D3DXVECTOR3(0,0,0);
}

HRESULT CPicking::Create(const D3DXMATRIX& matWorld,size_t iSize)
{
	m_matWorld = matWorld;
	return S_OK;
}

BOOL CPicking::Unprojection(LPDIRECT3DDEVICE9 pDevice, CCamera *pCamera,
							POINT *ptCursor, D3DXVECTOR3 *pvRayOrig, D3DXVECTOR3 *pvRayDir)
{
	D3DVIEWPORT9 vp;
	pDevice->GetViewport(&vp);	// 뷰포트 겟~

	D3DXMATRIXA16 matProj;
	matProj = pCamera->GetProjMatrix();
	//g_pApp->GetD3dDevice()->GetTransform( D3DTS_PROJECTION, &matProj );	// 프로젝션 겟

	D3DXVECTOR3 v;
	v.x = ( ((( (ptCursor->x - vp.X)*2.0f) / vp.Width) - 1.0f) - matProj._31 ) / matProj._11;
	v.y = (( -((( (ptCursor->y - vp.Y)*2.0f) / vp.Height) - 1.0f)) - matProj._32) / matProj._22;
	v.z = 1.0f;
	
	// 카메라와 구해진 점을 잇는 반직선 만들기
	D3DXMATRIXA16 m;
	D3DXMATRIX *pMatView;
	pMatView = pCamera->GetViewMatrix();
	//g_pApp->GetD3dDevice()->GetTransform( D3DTS_VIEW, &matView );
	D3DXMatrixInverse (&m, NULL, pMatView);

	m_vRayDir.x = (v.x*m._11) + (v.y*m._21) + (v.z*m._31);	
	m_vRayDir.y = (v.x*m._12) + (v.y*m._22) + (v.z*m._32);
	m_vRayDir.z = (v.x*m._13) + (v.y*m._23) + (v.z*m._33);
	
	if( pvRayDir != NULL )
	{	(*pvRayDir) = m_vRayDir; }

	m_vRayOrig.x = m._41;
	m_vRayOrig.y = m._42;
	m_vRayOrig.z = m._43;

	if( pvRayOrig != NULL )
	{	(*pvRayOrig) = m_vRayOrig;	}

	return TRUE;
}

DWORD CPicking::TriangleIntersectTerrain(D3DXVECTOR3 *pvRayOrig, D3DXVECTOR3 *pvRayDir)
{
	if(pvRayOrig)	m_vRayOrig = *pvRayOrig;
	if(pvRayDir)	m_vRayDir = *pvRayDir;
	
	int nIntersect = 0;
	
	DWORD i;

	TRIINDEX *		pIndices  = NULL;
	TERRAINVERTEX*	pVertices = NULL;

	FLOAT fBary1, fBary2;
	FLOAT fDist,fMinDist = 10000.0f;
	
	DWORD dwFaceCnt = m_pTerrain->GetCullTri();

	pVertices = m_pTerrain->GetVertex(0,0);
	pIndices = m_pTerrain->GetCullIndex();

	for(i=0; i< dwFaceCnt; ++i )
	{
		D3DXVECTOR3 v0 = pVertices[pIndices[i]._0].p;
		D3DXVECTOR3 v1 = pVertices[pIndices[i]._1].p;
		D3DXVECTOR3 v2 = pVertices[pIndices[i]._2].p;
		
		if( this->IntersectTriangle(m_vRayOrig, m_vRayDir, v0, v1, v2,&fDist, &fBary1, &fBary2 ) )
		{
			if( fDist < 0 ) continue; // -값은 버린다.

			++nIntersect;
			if( fDist < fMinDist )
			{
				m_vIntersectPt = m_vRayOrig + m_vRayDir * fDist;
				m_fDist = fMinDist = fDist;
				m_vTri[0] = v0;
				m_vTri[1] = v1;
				m_vTri[2] = v2;
			}
		}
	}//end for:i*/
	
	return nIntersect;
}

//-------------------------------------------------------------------------
//	Name. TriangleIntersect			: 피킹 채크..
//	[In][Out]	CPicking *cpPick	: 피킹된 삼각형의 각종 정보
//	return : true/false 삼각형 피킹되었는지
//-------------------------------------------------------------------------
DWORD CPicking::TriangleIntersect(D3DXVECTOR3 *pvRayOrig, D3DXVECTOR3 *pvRayDir)
{
	if(pvRayOrig)	m_vRayOrig = *pvRayOrig;
	if(pvRayDir)	m_vRayDir = *pvRayDir;

	LPD3DXMESH pMesh = m_pXFile->GetMesh();
	D3DXMATRIX *pMatWorld = m_pXFile->GetWorldMat();

	WORD*			pIndices;
	XFILE_VERTEX*	pVertices;

	// 락 거는게 맘에 걸리넹
	pMesh->LockIndexBuffer(0, (void**)&pIndices);
	pMesh->LockVertexBuffer(0, (void**)&pVertices);
	
	DWORD dwNumFaces = pMesh->GetNumFaces();
	FLOAT fBary1, fBary2;
	FLOAT fDist,fMinDist = 10000.0f;
	
	int nIntersect = 0;
	
	for(DWORD i=0; i<dwNumFaces; i++ )
	{
		D3DXVECTOR3 v0 = pVertices[pIndices[3*i+0]].vP;
		D3DXVECTOR3 v1 = pVertices[pIndices[3*i+1]].vP;
		D3DXVECTOR3 v2 = pVertices[pIndices[3*i+2]].vP;

		D3DXVec3TransformCoord(&v0,&v0,pMatWorld);
		D3DXVec3TransformCoord(&v1,&v1,pMatWorld);
		D3DXVec3TransformCoord(&v2,&v2,pMatWorld);
		
		// Check if the pick ray passes through this point
		if( this->IntersectTriangle(m_vRayOrig, m_vRayDir, v0, v1, v2,&fDist, &fBary1, &fBary2 ) )
		{
			if( fDist < 0 ) continue; // -값은 버린다.
			nIntersect++;
			if( fDist < fMinDist )
			{
				m_vIntersectPt = m_vRayOrig + m_vRayDir * fDist;
				m_fDist = fMinDist = fDist;
				m_vTri[0] = v0;
				m_vTri[1] = v1;
				m_vTri[2] = v2;
			}
		}
	}
	
	pMesh->UnlockIndexBuffer();
	pMesh->UnlockVertexBuffer();
	
	return nIntersect;
}

BOOL CPicking::IntersectTriangle( const D3DXVECTOR3& orig,const D3DXVECTOR3& dir, D3DXVECTOR3& v0,
								   D3DXVECTOR3& v1, D3DXVECTOR3& v2,FLOAT* t, FLOAT* u, FLOAT* v )
{
	    // Find vectors for two edges sharing vert0
    D3DXVECTOR3 edge1 = v1 - v0;
    D3DXVECTOR3 edge2 = v2 - v0;

    // Begin calculating determinant - also used to calculate U parameter
    D3DXVECTOR3 pvec;
    D3DXVec3Cross( &pvec, &dir, &edge2 );

    // If determinant is near zero, ray lies in plane of triangle
    FLOAT det = D3DXVec3Dot( &edge1, &pvec );

    D3DXVECTOR3 tvec;
    if( det > 0 )
    {
        tvec = orig - v0;
    }
    else
    {
        tvec = v0 - orig;
        det = -det;
    }

    if( det < 0.0001f )
        return FALSE;

    // Calculate U parameter and test bounds
    *u = D3DXVec3Dot( &tvec, &pvec );
    if( *u < 0.0f || *u > det )
        return FALSE;

    // Prepare to test V parameter
    D3DXVECTOR3 qvec;
    D3DXVec3Cross( &qvec, &tvec, &edge1 );

    // Calculate V parameter and test bounds
    *v = D3DXVec3Dot( &dir, &qvec );
    if( *v < 0.0f || *u + *v > det )
        return FALSE;

    // Calculate t, scale parameters, ray intersects triangle
    *t = D3DXVec3Dot( &edge2, &qvec );
    FLOAT fInvDet = 1.0f / det;
    *t *= fInvDet;
    *u *= fInvDet;
    *v *= fInvDet;

    return TRUE;
}

