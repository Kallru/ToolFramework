// File. Render_Cartoon.cpp

#include "StdAfx.h"
#include "Render_Cartoon.h"

CRender_Cartoon::CRender_Cartoon():
m_pSilDecl(NULL),
m_pSilhouetteVS(NULL),
m_pSurfaceVS(NULL),
m_pTexture(NULL),
m_pd3dDevice(NULL),
m_pMatWorld(NULL),
m_cpCamera(NULL)
{
	m_nType = RT_Cartoon;
	D3DXMatrixIdentity(&m_matTran1);
	D3DXMatrixIdentity(&m_matTran2);
}

CRender_Cartoon::~CRender_Cartoon()
{
	SAFE_RELEASE(m_pSilDecl);
	SAFE_RELEASE(m_pSilhouetteVS);
	SAFE_RELEASE(m_pSurfaceVS);
	SAFE_RELEASE(m_pTexture);
}

HRESULT CRender_Cartoon::CreateShaders(LPDIRECT3DDEVICE9 pDevice, CCamera* pCamera, 
						  LPSTR lpSilFileName,LPSTR lpSurFileName,LPSTR lpTexFileName, DWORD dwFVF)
{
	//----------------------------------------------------------------------------------------------------
	// Data Setting
	//----------------------------------------------------------------------------------------------------
	if( NULL == pDevice || NULL == pCamera ) return E_FAIL;
	m_pd3dDevice = pDevice;
	m_cpCamera = pCamera;

	//----------------------------------------------------------------------------------------------------
	// Create Sillhouette Shader~~ 
	//----------------------------------------------------------------------------------------------------
	LPD3DXBUFFER	pShader;
	LPD3DXBUFFER	pErrors;

	if(FAILED( D3DXAssembleShaderFromFile(lpSilFileName,NULL,NULL,NULL,&pShader,&pErrors) ))
	{
		OutputDebugString((char *)pErrors->GetBufferPointer());
		return E_FAIL; 
	}

	pErrors->Release();

	// decl을 만든다
	if( FAILED( this->SetFVF(dwFVF) ))
	{
		return E_FAIL;
	}

	// 생성 버텍스 쉐이덩~
    m_pd3dDevice->CreateVertexShader((DWORD *)pShader->GetBufferPointer(),&m_pSilhouetteVS);
	pShader->Release();

	//----------------------------------------------------------------------------------------------------
	// Create Surface Shader
	//----------------------------------------------------------------------------------------------------
	if(FAILED( D3DXAssembleShaderFromFile(lpSurFileName,NULL,NULL,NULL,&pShader,&pErrors) ))
	{
		OutputDebugString((char *)pErrors->GetBufferPointer());
		return E_FAIL; 
	}
	pErrors->Release();

    // 면 버텍스는 decl을 생성 안한당. (이미 위에서 만들어서)
	m_pd3dDevice->CreateVertexShader((DWORD *)pShader->GetBufferPointer(),&m_pSurfaceVS);
	pShader->Release();

	//----------------------------------------------------------------------------------------------------
	// Texture Load
	//----------------------------------------------------------------------------------------------------
	if( FAILED( D3DXCreateTextureFromFile(m_pd3dDevice,lpTexFileName,&m_pTexture) ))
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CRender_Cartoon::SetFVF(const DWORD& dwFVF)
{
	SAFE_RELEASE(m_pSilDecl);

	// decl을 만든다
	D3DVERTEXELEMENT9 decl[MAX_FVF_DECL_SIZE];
	D3DXDeclaratorFromFVF( dwFVF, decl );
	if( FAILED( m_pd3dDevice->CreateVertexDeclaration( decl, &m_pSilDecl ) ))
	{	return E_FAIL;	}

	return S_OK;
}

bool CRender_Cartoon::FrameMove()
{
	/*if( m_pMatWorld && m_cpCamera)
	{
		D3DXMATRIX matView = m_cpCamera->GetViewMatrix();
		D3DXMATRIX matProj = m_cpCamera->GetProjMatrix();
		D3DXMatrixTranspose( &m_matTran1, &( (*m_pMatWorld) * matView) );
		D3DXMatrixTranspose( &m_matTran2, &( (*m_pMatWorld) * matView * matProj) );
	}*/
	return true;
}

HRESULT CRender_Cartoon::BeginScene(D3DXMATRIX* pMat, D3DXVECTOR3& vLightDir, const D3DXVECTOR4& vC0)
{
	// 지형하고의 문제 발생!!
	m_pd3dDevice->SetRenderState( D3DRS_DITHERENABLE,   FALSE );
	m_pd3dDevice->SetRenderState( D3DRS_SPECULARENABLE, FALSE );
	m_pd3dDevice->SetRenderState( D3DRS_ZENABLE,        TRUE );
	m_pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE,	TRUE );
	m_pd3dDevice->SetRenderState( D3DRS_LIGHTING,		FALSE );
		
	m_pMatWorld = pMat;
	D3DXMATRIX *pMatView = m_cpCamera->GetViewMatrix();
	D3DXMATRIX matProj = m_cpCamera->GetProjMatrix();
	
	D3DXMatrixTranspose( &m_matTran1, &( (*m_pMatWorld) * (*pMatView)) );
	m_pd3dDevice->SetVertexShaderConstantF( 1, (float*)&m_matTran1, 4 );

	D3DXMatrixTranspose( &m_matTran2, &( (*m_pMatWorld) * (*pMatView) * matProj) );
	m_pd3dDevice->SetVertexShaderConstantF( 5, (float*)&m_matTran2, 4 );
	
	// Load light direction into shader constant 
	D3DXVec3TransformNormal( &vLightDir, &vLightDir, pMatView);

	float    ld[4] = {vLightDir.x,vLightDir.y,vLightDir.z,0};
	m_pd3dDevice->SetVertexShaderConstantF( 10, ld , 1 ); 
	m_pd3dDevice->SetVertexShaderConstantF( 0, vC0, 1 );

	m_pd3dDevice->SetVertexDeclaration(m_pSilDecl);

	return S_OK;
}

HRESULT CRender_Cartoon::SetSurfaceShader(const D3DXVECTOR4& vColor)
{
	m_pd3dDevice->SetVertexShader( m_pSurfaceVS );
	m_pd3dDevice->SetVertexShaderConstantF( 9 , vColor , 1 ); 
	
	m_pd3dDevice->SetTexture(0, m_pTexture );
	m_pd3dDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_NONE);
	m_pd3dDevice->SetTextureStageState( 0 , D3DTSS_COLOROP , D3DTOP_MODULATE ); 
	m_pd3dDevice->SetTextureStageState( 0 , D3DTSS_COLORARG1 , D3DTA_TEXTURE ); 
	m_pd3dDevice->SetTextureStageState( 0 , D3DTSS_COLORARG2 , D3DTA_DIFFUSE ); 
	m_pd3dDevice->SetTextureStageState( 1 , D3DTSS_COLOROP , D3DTOP_DISABLE ); 
	m_pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE , FALSE ); 

	return S_OK;
}

HRESULT CRender_Cartoon::SetSiluouetteShader(DWORD dwLineAmount/*=81*/)
{
	m_pd3dDevice->SetVertexShader( m_pSilhouetteVS ); 
	m_pd3dDevice->SetTextureStageState( 0 , D3DTSS_COLOROP , D3DTOP_SELECTARG1 ); 
	m_pd3dDevice->SetTextureStageState( 0 , D3DTSS_COLORARG1 , D3DTA_DIFFUSE ); 
	m_pd3dDevice->SetTextureStageState( 1 , D3DTSS_COLOROP , D3DTOP_DISABLE ); 
	m_pd3dDevice->SetTextureStageState( 0 , D3DTSS_ALPHAOP , D3DTOP_SELECTARG1 ); 
	m_pd3dDevice->SetTextureStageState( 0 , D3DTSS_ALPHAARG1 , D3DTA_DIFFUSE );
	m_pd3dDevice->SetTextureStageState( 1 , D3DTSS_ALPHAOP , D3DTOP_DISABLE ); 
	m_pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE , TRUE ); 
	m_pd3dDevice->SetRenderState( D3DRS_ALPHAFUNC , D3DCMP_GREATEREQUAL ); 
	m_pd3dDevice->SetRenderState( D3DRS_ALPHAREF , dwLineAmount ); 

	return S_OK;
}

HRESULT CRender_Cartoon::EndScene()
{
	m_pd3dDevice->SetVertexShader( NULL );
	m_pd3dDevice->SetTexture(0, NULL);
	m_pd3dDevice->SetTexture(1, NULL);

	m_pd3dDevice->SetTextureStageState( 0 , D3DTSS_COLORARG1 , D3DTA_TEXTURE);
	return S_OK;
}
