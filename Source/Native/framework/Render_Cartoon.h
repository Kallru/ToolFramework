// File. Render_Cartoon.h

#ifndef _RENDER_CARTOON_
#define _RENDER_CARTOON_

class CRender;
class CCamera;

// �� ���� ������ ī�� ���̴� �̷��� ������ -_-;
class CRender_Cartoon : public CRender
{
public:
	CRender_Cartoon();
	virtual ~CRender_Cartoon();

	// ������ ������ ī�� ���̱� ������, ���� �̸��� �����ε�...-_-;
	HRESULT CreateShaders(LPDIRECT3DDEVICE9 pDevice, CCamera* pCamera, 
						  LPSTR lpSilFileName,LPSTR lpSurFileName,LPSTR lpTexFileName, DWORD dwFVF);

	HRESULT SetFVF(const DWORD& dwFVF);
	HRESULT BeginScene(D3DXMATRIX* pMat, D3DXVECTOR3& vLightDir, const D3DXVECTOR4& vC0);
	HRESULT SetSurfaceShader(const D3DXVECTOR4& vColor);
	HRESULT SetSiluouetteShader(DWORD dwLineAmount=81);
	HRESULT EndScene();

	virtual bool FrameMove();

private:
	LPDIRECT3DVERTEXDECLARATION9	m_pSilDecl;
	LPDIRECT3DVERTEXSHADER9			m_pSilhouetteVS;
	LPDIRECT3DVERTEXSHADER9			m_pSurfaceVS;

	LPDIRECT3DDEVICE9				m_pd3dDevice;	// �� ���� �̱����� �ʿ��Ѱ�
	LPDIRECT3DTEXTURE9				m_pTexture;

	D3DXMATRIX		*m_pMatWorld;	// �ܺο��� ���� ������ǥ (�ش� ���� ����)
	D3DXMATRIX		m_matTran1;		// world * view
	D3DXMATRIX		m_matTran2;		// world * view * proj

	CCamera	*		m_cpCamera;
};

#endif
