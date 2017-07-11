//------------------------------------------------------------------------------
// File. Player.cpp
//------------------------------------------------------------------------------

#include "StdAfx.h"
#include "Player.h"

CPlayer::CPlayer():
m_pCamera(NULL),
m_vPos(0,0,0),
m_vLook(0,0,1),
m_vUpPt(0,1,0),
m_vRight(1,0,0),
m_pGun(NULL),
m_fCamHeight(0),
m_bFire(0),
m_fReaction(0),
m_fMapHeight(0)
{}

CPlayer::~CPlayer()
{
	SAFE_DELETE(m_pCamera );

	DWORD dwSize = m_pFireEffect.size();
	int i;
	for(i=0; i<dwSize; ++i)
	{
		SAFE_DELETE(m_pFireEffect[i]);
	}
	m_pFireEffect.clear();
}

HRESULT CPlayer::Create(LPDIRECT3DDEVICE9 pD3DDevice,CCamera *pCamera, const D3DXVECTOR3& vStartPos,ZTerrain * pTerrain)
{
	m_pTerrain   = pTerrain;
	m_fMoveSpeed = 3.0f; //5.0f;
	m_fCamHeight = 0.7f;//1.0f;
	//m_fMoveSpeed = 15.0f;
	//m_fCamHeight = 10.0f;

	m_pD3DDevice = pD3DDevice;
	m_pCamera	 = pCamera;
	m_pMoveTimer = new CTimer;
	m_pFireTimer = new CTimer;
	m_pFireTimer->Begin(); // 바로 돌려버림
	
	// 현재 CModel 구조에선, 같은 텍스처를 사용해도 공유하지 않는다.
	// 구조를  [ 텍스처 매니저<-> 모델 > 메쉬 ] 이런식으로 써야 할껏 같다.
	// 그리고 텍스처 매니저는 싱글톤으로 만들어서 모든 텍스처를 관리하는 것도 나쁘지 않을듯..
	// 다 만들면 한 세월이니 걍 합세 ㅋ
	m_pGun		 = new CModel(m_pD3DDevice);
	m_pGun->CreateModel(g_Utility.GetAssetPath("gun.MD3"),g_Utility.GetAssetPath("gun.skin" ) );
	m_pGun->SetCamera(m_pCamera);

	m_pGun->Scaling(0.01f,0.01f,0.01f);
	//m_pGun->Translation(0.8f,-0.8f,1.5f);
	m_pGun->Translation(0.008f,-0.008f,0.015f);

	m_pGunBarrel = new CModel(m_pD3DDevice);
	m_pGunBarrel->CreateModel(g_Utility.GetAssetPath("gunbarrel.MD3"),g_Utility.GetAssetPath("gunbarrel.skin"));
	m_pGunBarrel->SetCamera(m_pCamera);
	m_pGunBarrel->Scaling(0.01f,0.01f,0.01f);
	m_pGunBarrel->Translation(0.008f,-0.008f,0.023f);
	
	//------------------------------------------------------------------------------------
	// 이팩트를 생성시켜둔다. 
	// - 라이프 타임은 여기서 설정하기엔 좀 거시기 한 부분이 있음
	CEffect *pEffect = new CEffect_FireBroken(m_pD3DDevice);

	EFFECT_CREATEINFO tagInfo;
	tagInfo.dwFileNum = 1;
	tagInfo.lpFileName = g_Utility.GetAssetPath("effect\\bullet\\bulletHit.X");

	tagInfo.dwTexNum = 3;
	tagInfo.dwLifeTime = 200;

	char lpTexPath[3][128] = {0,};
	sprintf( &lpTexPath[0][0], "%s", g_Utility.GetAssetPath("effect\\bullet\\bullet1.jpg") );
	sprintf( &lpTexPath[1][0], "%s", g_Utility.GetAssetPath("effect\\bullet\\bullet1.jpg") );
	sprintf( &lpTexPath[2][0], "%s", g_Utility.GetAssetPath("effect\\bullet\\bullet1.jpg") );

	tagInfo.lpTexName = &lpTexPath[0][0];
	pEffect->Create(tagInfo);
	pEffect->Scaling(0.3f,0.3f,0.3f);
	m_pFireEffect.push_back(pEffect);
	pEffect=NULL;

	pEffect = new CEffect_Fire(m_pD3DDevice,m_pCamera);

	ZeroMemory(&tagInfo, sizeof(EFFECT_CREATEINFO));

	tagInfo.dwFileNum = 1;
	tagInfo.lpFileName = g_Utility.GetAssetPath("effect\\bullet\\flash.X");

	tagInfo.dwTexNum = 1;
	tagInfo.dwLifeTime = 50;
	tagInfo.lpTexName = g_Utility.GetAssetPath("effect\\bullet\\f_machinegun.jpg");

	pEffect->Create(tagInfo);
	m_pFireEffect.push_back(pEffect);

	//-------------------------------------------------------------------------------------
	// 플레이어 초기 셋팅
	// 초기 포지션
	this->SetPos(D3DXVECTOR3(64,0,64));

	// 이동시간 
	m_pMoveTimer->Start();
	return S_OK;
}

void CPlayer::SetPos(D3DXVECTOR3 vPos)
{
	D3DXVECTOR3 vEyePt = vPos;

	// 맵의 y값을 얻는다
	// y값 보간은 나중으로 미루자
	m_fMapHeight = m_pTerrain->GetHeightEx(vEyePt.x,vEyePt.z)+0.1f;

	// 카메라가 서야될 높이를 적용
	vEyePt.y = vPos.y + m_fCamHeight + m_fMapHeight;
	
	// 최종 위치
	m_pCamera->MoveTo(&vEyePt);
	
	m_vPos = vPos;
}

bool CPlayer::Move(char szKey)
{
	assert(m_pCamera != NULL);	// 카메라가 없으면 무조건 Error
	
	DWORD dwTime = m_pMoveTimer->GetElaps()*1000;
	float fDist = CUtility::FrameSecDist(m_fMoveSpeed,dwTime);

	D3DXVECTOR3 vDir(0,0,0);

	switch(szKey)
	{
	case 'W':	vDir = m_vLook;		break;
	case 'S':	vDir = -m_vLook;	break;
	case 'A':	vDir = -m_vRight;	break;
	case 'D':	vDir = m_vRight;	break;
	}
	
	D3DXVECTOR3 vMove;	
	D3DXVec3Normalize(&vMove,&vDir);

	//vMove *= fDist;

	D3DXVECTOR3 vPos = m_vPos + (vMove * fDist);
	float fMapHeight = m_pTerrain->GetHeightEx(vPos.x,vPos.z)+0.1f;

	float fGap = fMapHeight - m_fMapHeight;
	if( 0 < fGap )
	{
		float fDelta=0.5f;

		fDist -= (fGap*fDelta);

		if( fDist < 0 ) fDist = 0;

		vPos = m_vPos + (vMove * fDist);
	}

	// 최종적으로 적용되는건 플레이어 위치이다.
	//m_vPos += vMove;

	// 플레이어 위치를 줘야 함(카메라 위치는 안에서 계산한다)
	this->SetPos(vPos);

	return true;
}

bool CPlayer::Roate(char szType, float fAngle)
{
	assert(m_pCamera != NULL);
	if('x' == szType)
	{
		m_pCamera->RotateLocalX(fAngle);
	}
	else if('y' == szType)
	{
		m_pCamera->RotateLocalY(fAngle);

		m_vRight = m_pCamera->GetRightPt();
		m_vLook = m_pCamera->GetViewDir();
		m_vLook.y = 0;
	}

	return true;
}

bool CPlayer::FrameMove(float FrameDelta)
{
	if(!m_pFireEffect.empty())
	{
		vector<CEffect *>::iterator iter = m_pFireEffect.begin();
		vector<CEffect *>::iterator itEnd = m_pFireEffect.end();

		for(;iter != itEnd; ++iter)
		{
			(*iter)->FrameMove();
		}
	}

	this->Act_Attack();
	return true;
}

bool CPlayer::Render()
{
	this->SetLight();

	if(!m_pFireEffect.empty())
	{
		vector<CEffect *>::iterator iter = m_pFireEffect.begin();
		vector<CEffect *>::iterator itEnd = m_pFireEffect.end();

		for(;iter != itEnd; ++iter)
		{
			(*iter)->Render();
		}
	}

//	m_pD3DDevice->LightEnable(0,TRUE);
//	m_pD3DDevice->SetRenderState(D3DRS_AMBIENT, 0x00909090);
	
	m_pGun->Render();

	m_pGunBarrel->Render();

	return true;
}

bool CPlayer::SetLight()
{
	D3DMATERIAL9 mtrl;
	ZeroMemory(&mtrl, sizeof(D3DMATERIAL9));
	mtrl.Diffuse.r = mtrl.Ambient.r = 1.0f;
	mtrl.Diffuse.g = mtrl.Ambient.g = 1.0f;
	mtrl.Diffuse.b = mtrl.Ambient.b = 1.0f;
	mtrl.Diffuse.a = mtrl.Ambient.a = 1.0f;
	m_pD3DDevice->SetMaterial(&mtrl);

/*	// 라이트 설정
	D3DXVECTOR3 vecDir;
	D3DLIGHT9 light;
	ZeroMemory(&light, sizeof(D3DLIGHT9));
	light.Type = D3DLIGHT_DIRECTIONAL; // 다이랙션 광원~
//	light.Type = D3DLIGHT_POINT;
	light.Diffuse.r = 1.0f;
	light.Diffuse.g = 1.0f;
	light.Diffuse.b = 1.0f;
	light.Range = 1000.0f;
	light.Position = D3DXVECTOR3(0,10,0);
	vecDir = D3DXVECTOR3(0.0f,-1.0f,-1.0f);
	D3DXVec3Normalize((D3DXVECTOR3*)&light.Direction, &vecDir);
	m_pD3DDevice->SetLight(0, &light);
	m_pD3DDevice->LightEnable(0, TRUE);
	m_pD3DDevice->SetRenderState( D3DRS_AMBIENT, D3DCOLOR_XRGB(120,120,120));

  /**/
	return true;
}

// 상태는 일단 간단하게 디자인하자
bool CPlayer::State(PlayerState state)
{
	switch( state )
	{
	case PS_Unkown:		break;
	case PS_FireStart:	m_bFire = true;		break;
	case PS_FireStop:	m_bFire = false;	break;
	}

	return true;
}

bool CPlayer::Act_Attack()
{
	if( !m_bFire ) return false;	// Fire가 false라면 리턴해버림

	// 연사 속도를 적용해야함
	if( m_pFireTimer->GetTime() < 0.1 )
	{
		return false;
	}

	float fDist = CUtility::FrameSecDist(10,100);
	D3DXVECTOR3 vAxisZ(0,0,1);
	static float a=0;
	a+=fDist;
	m_pGunBarrel->RotateAxis(&vAxisZ,a);

	// 일단 임시로 반동을 이렇게 구현해놓자 ㅋ
	static bool bTest =false;

	if(bTest)	m_fReaction=0.001f;
	else		m_fReaction=0;
	bTest = !bTest;

	m_pGun->Translation(0.008f,-0.008f,0.015f+m_fReaction);
	m_pGunBarrel->Translation(0.008f,-0.008f,0.023f+m_fReaction);

	m_pFireTimer->Begin();

	// 이팩트 설정~~

	EFFECT_INFO tagInfo;

	vector<CEffect *>::iterator iter = m_pFireEffect.begin();
	vector<CEffect *>::iterator itEnd = m_pFireEffect.end();

	for(;iter != itEnd; ++iter)
	{
		ZeroMemory(&tagInfo,sizeof(EFFECT_INFO));
		switch((*iter)->GetType())
		{
		case ET_FireBroken:
			{
				tagInfo.vPos = m_pPick->m_vIntersectPt;		// 이팩트 시작 지점
				
				// 노멀 구하기
				D3DXVECTOR3 vN;
				D3DXVECTOR3 v1 = m_pPick->m_vTri[0] - m_pPick->m_vTri[1];
				D3DXVECTOR3 v2 = m_pPick->m_vTri[2] - m_pPick->m_vTri[1];
				
				D3DXVec3Cross(&vN,&v1,&v2);
				D3DXVec3Normalize(&tagInfo.vDir, &vN);
			}
			break;
		case ET_Firegun:
			{
				tagInfo.vPos = D3DXVECTOR3(0.3f,-0.3f,2.3f+m_fReaction);
			}
			break;
		default: return false;	// 이팩트 실패
		}

		(*iter)->SetEffect(tagInfo);
	}

	return true;
}

bool CPlayer::Collision(CPicking *pPick)
{
	m_pPick = pPick;	// 피킹된 지점 (오브젝 또는 지형)에 대한 정보를 가지고 있다.
	return true;
}
