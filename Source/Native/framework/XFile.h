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
	// ���� ���� ������Ʈ�� �����ϸ�, ���� ������ (�Ǵ� �ܺ� ������ ����ؼ� �����Ѵ�)
	//--------------------------------------------------------------------------------------
	bool	Render(CRender * pRender=NULL);

	//--------------------------------------------------------------------------------------
	// Name. bool DrawSubset();
	// �޽��� ������ �ؽ�ó�� ���� �Ŀ� �����Ѵ�. ����������Ʈ�� �ܺο��� ��������� �Ѵ�.
	//--------------------------------------------------------------------------------------
	bool	DrawSubset();

	void SetTranslation(float x, float y, float z) { this->SetTranslation(D3DXVECTOR3(x,y,z)); }
	void SetTranslation(const D3DXVECTOR3& vPos) { m_vPos = vPos; }

	void SetScaling(float x,float y, float z) { D3DXMatrixScaling(&m_matScale,x,y,z); }
	void SetRotation(const D3DXVECTOR3& vAxis, float fAngle) { D3DXQuaternionRotationAxis(&m_quateRotate,&vAxis, fAngle); }

	DWORD GetFVF() { if(m_pMesh) return m_pMesh->GetFVF(); return 0; }

	LPD3DXMESH GetMesh() { return m_pMesh; }

	D3DXMATRIX *GetWorldMat() { return &m_matWorld; }

	// �ӽ�
	void SetInverseView(D3DXMATRIX *pMat) { m_pInverseView = pMat; }

private:
	void SetWorldTransform();
	
private:
	D3DXVECTOR3 m_vPos;

	bool m_bCartoon;

	LPDIRECT3DTEXTURE9	m_pSetTexture;	// �ܺο��� �ִ� �ؽ�ó -0-;; �� ������~
	LPDIRECT3DTEXTURE9	m_pUserTexture;
	LPDIRECT3DDEVICE9	m_pD3DDevice;	// �����͸� �޾Ƶ�

	D3DXMATRIX			m_matWorld;
	D3DXMATRIX			m_matScale;
	D3DXMATRIX			*m_pInverseView;
	D3DXQUATERNION		m_quateRotate;

	D3DXMATRIX			m_matRotea;
	LPD3DXMESH			m_pMesh;		// ���߿� ���� �޽��� ������..
	LPDIRECT3DTEXTURE9*	m_pTex;
	DWORD				m_dwNumMaterials;
	D3DMATERIAL9*		m_pMaterials;	
};

#endif