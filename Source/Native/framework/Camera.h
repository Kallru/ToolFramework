//------------------------------------------------------------------------------------------
//	File. Camera.h
//------------------------------------------------------------------------------------------

#ifndef _CAMERA_
#define _CAMERA_

class CCamera
{
public:
	CCamera();
	~CCamera();
	
	
	D3DXMATRIX * SetViewMatrix( D3DXVECTOR3 &vEyePt, D3DXVECTOR3& vLookatPt,
		                        D3DXVECTOR3& vUpVec );
    D3DXMATRIX * SetProjection( FLOAT fFOV, FLOAT fAspect, FLOAT fNearPlane,
				                FLOAT fFarPlane );

	D3DXVECTOR3 GetEyePt()           { return m_vEyePt; }
    D3DXVECTOR3 GetLookatPt()        { return m_vLookatPt; }
    D3DXVECTOR3 GetUpVec()           { return m_vUpVec; }
    D3DXVECTOR3 GetViewDir()         { return m_vView; }
	D3DXVECTOR3 GetRightPt()         { return m_vRight; }
	D3DXMATRIX *GetViewMatrix()      { return &m_matView; }
	D3DXMATRIX  GetProjMatrix()      { return m_matProj; }

	void		RotateLocalX( float angle );
	void		RotateLocalY( float angle );
	void		MoveTo( D3DXVECTOR3* pv );
	void		MoveLocalX( float dist );
	void		MoveLocalY( float dist );
	void		MoveLocalZ( float dist );
private:

	D3DXVECTOR3 m_vEyePt;       // Attributes for view matrix
    D3DXVECTOR3 m_vLookatPt;
    D3DXVECTOR3 m_vUpVec;
	D3DXVECTOR3 m_vRight;
	
    D3DXMATRIXA16	m_matProj;
	D3DXVECTOR3		m_vView;
    D3DXMATRIXA16	m_matView;
	
	FLOAT       m_fFOV;         // Attributes for projection matrix
    FLOAT       m_fAspect;
    FLOAT       m_fNearPlane;
    FLOAT       m_fFarPlane;
};

#endif