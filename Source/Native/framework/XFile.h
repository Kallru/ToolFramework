//------------------------------------------------------------------------------
// File. XFile.h
//------------------------------------------------------------------------------

#ifndef _XFILE_
#define _XFILE_

typedef struct 
{
	D3DXVECTOR3 vP;
	D3DXVECTOR3 vN;
	float tu,tv;
}XFILE_VERTEX;

class CRender;
class CRender_Cartoon;

class CXFile
{
public:
	CXFile();
	~CXFile();

	HRESULT Create(char *strFileName,LPDIRECT3DDEVICE9 Device, bool bCartoon);
	HRESULT	CreateTexture(char *strFileName);

	void SetTexture(LPDIRECT3DTEXTURE9 pTexture) { m_pSetTexture = pTexture; }

	//--------------------------------------------------------------------------------------
	// Name. bool Render();
	// 직접 랜더 스테이트를 관리하며, 모델을 랜더함 (또는 외부 랜더를 사용해서 랜더한다)
	//--------------------------------------------------------------------------------------
	bool	Render(CRender * pRender=NULL);

	//--------------------------------------------------------------------------------------
	// Name. bool DrawSubset();
	// 메쉬의 재질과 텍스처만 셋팅 후에 랜더한다. 랜더스테이트는 외부에서 설정해줘야 한다.
	//--------------------------------------------------------------------------------------
	bool	DrawSubset();

	void SetTranslation(float x, float y, float z) { this->SetTranslation(D3DXVECTOR3(x,y,z)); }
	void SetTranslation(const D3DXVECTOR3& vPos) { m_vPos = vPos; }

	void SetScaling(float x,float y, float z) { D3DXMatrixScaling(&m_matScale,x,y,z); }
	void SetRotation(const D3DXVECTOR3& vAxis, float fAngle) { D3DXQuaternionRotationAxis(&m_quateRotate,&vAxis, fAngle); }

	DWORD GetFVF() { if(m_pMesh) return m_pMesh->GetFVF(); return 0; }

	LPD3DXMESH GetMesh() { return m_pMesh; }

	D3DXMATRIX *GetWorldMat() { return &m_matWorld; }

	// 임시
	void SetInverseView(D3DXMATRIX *pMat) { m_pInverseView = pMat; }

private:
	void SetWorldTransform();
	
private:
	D3DXVECTOR3 m_vPos;

	bool m_bCartoon;

	LPDIRECT3DTEXTURE9	m_pSetTexture;	// 외부에서 주는 텍스처 -0-;; 걍 대충해~
	LPDIRECT3DTEXTURE9	m_pUserTexture;
	LPDIRECT3DDEVICE9	m_pD3DDevice;	// 포인터만 받아둠

	D3DXMATRIX			m_matWorld;
	D3DXMATRIX			m_matScale;
	D3DXMATRIX			*m_pInverseView;
	D3DXQUATERNION		m_quateRotate;

	D3DXMATRIX			m_matRotea;
	LPD3DXMESH			m_pMesh;		// 나중에 따로 메쉬로 빼던가..
	LPDIRECT3DTEXTURE9*	m_pTex;
	DWORD				m_dwNumMaterials;
	D3DMATERIAL9*		m_pMaterials;	
};

#endif