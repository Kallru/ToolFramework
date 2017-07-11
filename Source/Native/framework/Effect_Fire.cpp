// Effect_Fire.cpp: implementation of the CEffect_Fire class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Effect_Fire.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CEffect_Fire::CEffect_Fire(LPDIRECT3DDEVICE9 pDevice,CCamera *pCamera)
{
	m_pd3dDevice = pDevice;
	m_pCamera = pCamera;
	m_Type = ET_Firegun;
}

CEffect_Fire::~CEffect_Fire()
{

}

HRESULT CEffect_Fire::Create(const EFFECT_CREATEINFO& tagCreate)
{
	m_tagCreateInfo = tagCreate;
	m_pModel = new CXFile;
	if(FAILED( m_pModel->Create(&tagCreate.lpFileName[0],m_pd3dDevice,false)) )
	{	return E_FAIL;	}	// 로그를 남겨둬야 하는데~

	//m_pModel->SetScaling(0.1f,0.1f,0.1f);

	if(FAILED(D3DXCreateTextureFromFile(m_pd3dDevice,&tagCreate.lpTexName[0],&m_pTex)) )
	{	return E_FAIL;	}
	return S_OK;
}

bool CEffect_Fire::SetEffect(const EFFECT_INFO& tagInfo)
{
	if( m_listInfo.empty() )
	{
		EFFECT_INFO *pInfo = new EFFECT_INFO;
		*pInfo = tagInfo;
		
		pInfo->bActive = true;
		pInfo->m_Timer.Begin();
		m_listInfo.push_back(pInfo);

		return true;
	}

	return false;
}

bool CEffect_Fire::FrameMove()
{
	list<EFFECT_INFO*>::iterator iter = m_listInfo.begin();
	list<EFFECT_INFO*>::iterator itEnd = m_listInfo.end();

	for(; iter != itEnd; ++iter)
	{
		EFFECT_INFO *pInfo = (*iter);

		if(!pInfo->bActive) continue;

		DWORD dwTime = pInfo->m_Timer.GetTime() * 1000;
		
		// 흐른 시간이 더 크다면..
		if( dwTime > m_tagCreateInfo.dwLifeTime )
		{
			SAFE_DELETE( *iter );
			m_listInfo.erase(iter);
			return false;
		}
	}
	return true;
}

bool CEffect_Fire::Render()
{
	if( m_listInfo.empty() ) return false;

	D3DXMATRIX matInView = *m_pCamera->GetViewMatrix();
	D3DXMatrixInverse(&matInView, NULL, &matInView);
	m_pModel->SetInverseView(&matInView);
	
	EFFECT_INFO *pInfo = m_listInfo.front();

	// 회전~
//	m_pModel->SetRotation(vUp,fAngle);	
	m_pModel->SetTranslation(pInfo->vPos);
	m_pModel->SetTexture(m_pTex);
	
	// Render State
	m_pd3dDevice->SetRenderState( D3DRS_ZENABLE,		FALSE );
	m_pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE,	FALSE );
	m_pd3dDevice->SetRenderState( D3DRS_FOGENABLE,		FALSE );
	m_pd3dDevice->SetRenderState( D3DRS_CULLMODE,		D3DCULL_CW );
	//m_pd3dDevice->SetRenderState( D3DRS_LIGHTING,		TRUE );	// 발광체 셋팅하삼
	
	m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,  TRUE);
	m_pd3dDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_ONE );
	m_pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE );

	m_pd3dDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
	m_pd3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );

	m_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	m_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
	m_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
	
	m_pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	m_pd3dDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
	
	m_pModel->DrawSubset();

	m_pd3dDevice->SetTextureStageState(0, D3DTSS_COLOROP,	D3DTOP_MODULATE);
	m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,	FALSE);
	return true;
}