//------------------------------------------------------------------------------------------
//	File. Camera.cpp
//------------------------------------------------------------------------------------------

#include "StdAfx.h"
#include "Camera.h"

CCamera::CCamera()
{
	D3DXVECTOR3 vEyePt(0.0f,0.0f,0.0f);
	D3DXVECTOR3 vLookatPt(0.0f,0.0f,-1.0f);
	D3DXVECTOR3 vUpVec(0.0f,1.0f,0.0f);

	D3DXMatrixIdentity( &m_matView );
	
	SetViewMatrix( vEyePt, vLookatPt, vUpVec );
    SetProjection( D3DX_PI/4, 1.0f, 1.0f, 1000.0f );
}

CCamera::~CCamera()
{
}

D3DXMATRIX * CCamera::SetViewMatrix( D3DXVECTOR3 &vEyePt, D3DXVECTOR3& vLookatPt,D3DXVECTOR3& vUpVec )
{
    m_vEyePt    = vEyePt;
    m_vLookatPt = vLookatPt;
    m_vUpVec    = vUpVec;

	D3DXVECTOR3 Dir = m_vLookatPt - m_vEyePt;
	D3DXVec3Normalize(&m_vView,&Dir);

	D3DXVec3Cross(&m_vRight,&m_vUpVec,&m_vView);
	
	return D3DXMatrixLookAtLH( &m_matView, &m_vEyePt, &m_vLookatPt, &m_vUpVec );
}

D3DXMATRIX * CCamera::SetProjection( FLOAT fFOV, FLOAT fAspect, FLOAT fNearPlane,FLOAT fFarPlane )
{
	m_fFOV        = fFOV;
    m_fAspect     = fAspect;
    m_fNearPlane  = fNearPlane;
    m_fFarPlane   = fFarPlane;

    return D3DXMatrixPerspectiveFovLH( &m_matProj, fFOV, fAspect, fNearPlane, fFarPlane );
}

void CCamera::RotateLocalX( float angle )
{	
	D3DXMATRIXA16 matRot;
	D3DXMatrixRotationAxis( &matRot, &m_vRight, angle );

	D3DXVECTOR3 vNewDst;
	D3DXVec3TransformCoord( &vNewDst, &m_vView, &matRot );
	vNewDst += m_vEyePt;

	SetViewMatrix( m_vEyePt, vNewDst, m_vUpVec );
}

void CCamera::RotateLocalY( float angle )
{
	D3DXMATRIXA16 matRot;
	D3DXMatrixRotationAxis( &matRot, &m_vUpVec, angle );

	D3DXVECTOR3 vNewDst;
	D3DXVec3TransformCoord( &vNewDst, &m_vView, &matRot );
	vNewDst += m_vEyePt;

	SetViewMatrix( m_vEyePt, vNewDst, m_vUpVec );
}

void CCamera::MoveTo( D3DXVECTOR3* pv )
{
	D3DXVECTOR3	dv = *pv - m_vEyePt;
	m_vEyePt = *pv;
	m_vLookatPt += dv;
	
	SetViewMatrix( m_vEyePt, m_vLookatPt, m_vUpVec );
}

void CCamera::MoveLocalX( float dist )
{
	D3DXVECTOR3 vNewEye	= m_vEyePt;
	D3DXVECTOR3 vNewDst	= m_vLookatPt;
	
	D3DXVECTOR3 vMove;
	D3DXVec3Normalize( &vMove, &m_vRight );
	vMove	*= dist;
	vNewEye += vMove;
	vNewDst += vMove;
	
	SetViewMatrix( vNewEye, vNewDst, m_vUpVec );
}

void CCamera::MoveLocalY( float dist )
{
	D3DXVECTOR3 vNewEye	= m_vEyePt;
	D3DXVECTOR3 vNewDst	= m_vLookatPt;
	
	D3DXVECTOR3 vMove;
	D3DXVec3Normalize( &vMove, &m_vUpVec );
	vMove	*= dist;
	vNewEye += vMove;
	vNewDst += vMove;
	
	SetViewMatrix( vNewEye, vNewDst, m_vUpVec );
}

void CCamera::MoveLocalZ( float dist )
{
	D3DXVECTOR3 vNewEye	= m_vEyePt;
	D3DXVECTOR3 vNewDst	= m_vLookatPt;
	
	D3DXVECTOR3 vMove;
	D3DXVec3Normalize( &vMove, &m_vView );
	vMove	*= dist;
	vNewEye += vMove;
	vNewDst += vMove;
	
	SetViewMatrix( vNewEye, vNewDst, m_vUpVec );
}
