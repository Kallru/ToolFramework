// File. Effect_FireBroken.cpp

#include "StdAfx.h"
#include "Effect_FireBroken.h"

CEffect_FireBroken::CEffect_FireBroken(LPDIRECT3DDEVICE9 pDevice)
{
	m_pd3dDevice = pDevice;
	m_Type = ET_FireBroken;
}

CEffect_FireBroken::~CEffect_FireBroken()
{
	DWORD dwSize = m_listModel.size();
	int i;
	for(i=0; i<dwSize; ++i)
	{
		SAFE_DELETE(m_listModel[i]);
	}
	m_listModel.clear();

	dwSize = m_listSprite.size();
	for(i=0; i<dwSize; ++i)
	{
		SAFE_RELEASE(m_listSprite[i]->pTex);
	}
	m_listSprite.clear();
	//SAFE_DELETE(m_pTimer);
}

HRESULT CEffect_FireBroken::Create(const EFFECT_CREATEINFO& tagCreate)
{
	int i;
	for(i=0; i<tagCreate.dwFileNum; ++i)	// ���� �������� �� ���ɼ��� �����ؼ�..
	{
		CXFile *pModel = new CXFile;
		pModel->Create(&tagCreate.lpFileName[i],m_pd3dDevice,false);
		m_listModel.push_back(pModel);
	}

	DWORD dwMSP = tagCreate.dwLifeTime / tagCreate.dwTexNum;
	for(i=0; i<tagCreate.dwTexNum; ++i)
	{
		// �ؽ�ó ��� ������ �޴� �κ� �ذ� ���
		this->AddTexture(&tagCreate.lpTexName[i*128],dwMSP);
	}

	return S_OK;
}
bool CEffect_FireBroken::Scaling(float x,float y,float z)
{
	int i;
	DWORD dwSize = m_listModel.size();
	for(i=0; i<dwSize; ++i)
	{
		m_listModel[i]->SetScaling(x,y,z);
	}
	return true;
}

bool CEffect_FireBroken::SetEffect(const EFFECT_INFO& tagInfo)
{
	EFFECT_INFO *pInfo = new EFFECT_INFO;
	*pInfo = tagInfo;

	pInfo->bActive = true;
	pInfo->m_Timer.Begin();
	m_listInfo.push_back(pInfo);

	//m_pTimer = new CTimer;
	//m_pTimer->Begin();

	return true;
}

HRESULT CEffect_FireBroken::AddTexture(LPSTR lpFileName, DWORD dwMillSecTime)
{
	SPRITE *pSprite = new SPRITE;
	if(FAILED(D3DXCreateTextureFromFile(m_pd3dDevice,lpFileName,&pSprite->pTex)) )
	{
		return E_FAIL;
	}

	pSprite->dwMillSec = dwMillSecTime;

	this->AddTexture(pSprite);
	return S_OK;
}

HRESULT CEffect_FireBroken::AddTexture(SPRITE *pSprite)
{
	pSprite->dwIndex = m_listSprite.size();
	m_listSprite.push_back(pSprite);
	return S_OK;
}

bool CEffect_FireBroken::Render()
{
	list<EFFECT_INFO*>::iterator iter = m_listInfo.begin();
	list<EFFECT_INFO*>::iterator itEnd = m_listInfo.end();

	float fAngle(0);

//	int x = 5; //(rand()%2)+4;
//	int y = 5; //(rand()%2)+4;
//	int z = 5; //(rand()%2)+4;
	
	for(;iter != itEnd; ++iter)
	{
		EFFECT_INFO *pInfo = (*iter);
		if( !pInfo->bActive ) continue;
		
		// ���� �����Ϳ� ��ŷ�� ������ ��� ���Ϳ� �� ���̰��� ���� ��,
		// �κ����� ũ�ν�(����)�� ������ ���̰� ��ŭ ȸ���Ѵ�.
		D3DXVECTOR3 vUp(0,1,0);	// ���� �� ���͵� ���⿡ �����Ѵ�. (���� �ϳ� �Ƴ��� ������)
		fAngle = CUtility::AngleBetweenV(vUp,pInfo->vDir);	// ���̰� ���ϱ�
		
		// ���� ȸ�� ���� �������
		D3DXVec3Cross(&vUp,&vUp,&pInfo->vDir);
		
		// ȸ��~
		m_listModel[pInfo->dwCurMeshFrame]->SetRotation(vUp,fAngle);		
		m_listModel[pInfo->dwCurMeshFrame]->SetTranslation(pInfo->vPos);		
		m_listModel[pInfo->dwCurMeshFrame]->SetTexture(m_listSprite[pInfo->dwCurTexFrame]->pTex);
		
		// Render State
		m_pd3dDevice->SetRenderState( D3DRS_ZENABLE,		FALSE );
		m_pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE,	FALSE );
		m_pd3dDevice->SetRenderState( D3DRS_FOGENABLE,		FALSE );
		m_pd3dDevice->SetRenderState( D3DRS_CULLMODE,		D3DCULL_NONE );
		//m_pd3dDevice->SetRenderState( D3DRS_LIGHTING,		TRUE );	// �߱�ü �����ϻ�
		
		m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,  TRUE);
		m_pd3dDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_ONE );
		m_pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE );
		
		m_pd3dDevice->SetTextureStageState(0, D3DTSS_COLOROP,   D3DTOP_MODULATE2X );	// MODULATE2�� ���´�.
		m_pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE );		// �ؽ�ó
		//	m_pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_CURRENT);
		
		//	m_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
		//	m_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
		
		m_listModel[pInfo->dwCurMeshFrame]->DrawSubset();
	}

	m_pd3dDevice->SetTextureStageState(0, D3DTSS_COLOROP,	D3DTOP_MODULATE);
	m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,	FALSE);

	return true;
}

bool CEffect_FireBroken::FrameMove()
{
	list<EFFECT_INFO*>::iterator iter = m_listInfo.begin();
	list<EFFECT_INFO*>::iterator itEnd = m_listInfo.end();

	for(; iter != itEnd; ++iter)
	{
		EFFECT_INFO *pInfo = (*iter);

		if(!pInfo->bActive) continue;

		DWORD dwTime = pInfo->m_Timer.GetTime() * 1000;
		
		// �帥 �ð��� �� ũ�ٸ�..
		DWORD Test = m_listSprite[pInfo->dwCurTexFrame]->dwMillSec;
		if( dwTime > m_listSprite[pInfo->dwCurTexFrame]->dwMillSec )
		{
			// ����������
			++pInfo->dwCurTexFrame;
			pInfo->m_Timer.Begin();
			
			// ���� �������� ������ �������̶��
			if( m_listSprite.size() <= pInfo->dwCurTexFrame )
			{
				SAFE_DELETE( *iter );
				m_listInfo.erase(iter);
				return false;

				//pInfo->dwCurTexFrame = 0;
				//pInfo->bActive = false;
			}
		}
	}

	return true;
}
