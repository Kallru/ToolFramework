// File. Render_Cartoon.h

#ifndef _RENDER_CARTOON_
#define _RENDER_CARTOON_

class CRender;
class CCamera;

// 뭐 쓰는 랜더가 카툰 뿐이니 이렇게 만들자 -_-;
class CRender_Cartoon : public CRender
{
public:
	CRender_Cartoon();
	virtual ~CRender_Cartoon();

	// 어차피 랜더는 카툰 뿐이기 때문에, 파일 이름도 고정인데...-_-;
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

	LPDIRECT3DDEVICE9				m_pd3dDevice;	// 음 역시 싱글톤이 필요한가
	LPDIRECT3DTEXTURE9				m_pTexture;

	D3DXMATRIX		*m_pMatWorld;	// 외부에서 받을 월드좌표 (해당 모델의 월드)
	D3DXMATRIX		m_matTran1;		// world * view
	D3DXMATRIX		m_matTran2;		// world * view * proj

	CCamera	*		m_cpCamera;
};

#endif
