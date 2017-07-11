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
	m_pFireTimer->Begin(); // �ٷ� ��������
	
	// ���� CModel ��������, ���� �ؽ�ó�� ����ص� �������� �ʴ´�.
	// ������  [ �ؽ�ó �Ŵ���<-> �� > �޽� ] �̷������� ��� �Ҳ� ����.
	// �׸��� �ؽ�ó �Ŵ����� �̱������� ���� ��� �ؽ�ó�� �����ϴ� �͵� ������ ������..
	// �� ����� �� �����̴� �� �ռ� ��
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
	// ����Ʈ�� �������ѵд�. 
	// - ������ Ÿ���� ���⼭ �����ϱ⿣ �� �Žñ� �� �κ��� ����
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
	// �÷��̾� �ʱ� ����
	// �ʱ� ������
	this->SetPos(D3DXVECTOR3(64,0,64));

	// �̵��ð� 
	m_pMoveTimer->Start();
	return S_OK;
}

void CPlayer::SetPos(D3DXVECTOR3 vPos)
{
	D3DXVECTOR3 vEyePt = vPos;

	// ���� y���� ��´�
	// y�� ������ �������� �̷���
	m_fMapHeight = m_pTerrain->GetHeightEx(vEyePt.x,vEyePt.z)+0.1f;

	// ī�޶� ���ߵ� ���̸� ����
	vEyePt.y = vPos.y + m_fCamHeight + m_fMapHeight;
	
	// ���� ��ġ
	m_pCamera->MoveTo(&vEyePt);
	
	m_vPos = vPos;
}

bool CPlayer::Move(char szKey)
{
	assert(m_pCamera != NULL);	// ī�޶� ������ ������ Error
	
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

	// ���������� ����Ǵ°� �÷��̾� ��ġ�̴�.
	//m_vPos += vMove;

	// �÷��̾� ��ġ�� ��� ��(ī�޶� ��ġ�� �ȿ��� ����Ѵ�)
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

/*	// ����Ʈ ����
	D3DXVECTOR3 vecDir;
	D3DLIGHT9 light;
	ZeroMemory(&light, sizeof(D3DLIGHT9));
	light.Type = D3DLIGHT_DIRECTIONAL; // ���̷��� ����~
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

// ���´� �ϴ� �����ϰ� ����������
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
	if( !m_bFire ) return false;	// Fire�� false��� �����ع���

	// ���� �ӵ��� �����ؾ���
	if( m_pFireTimer->GetTime() < 0.1 )
	{
		return false;
	}

	float fDist = CUtility::FrameSecDist(10,100);
	D3DXVECTOR3 vAxisZ(0,0,1);
	static float a=0;
	a+=fDist;
	m_pGunBarrel->RotateAxis(&vAxisZ,a);

	// �ϴ� �ӽ÷� �ݵ��� �̷��� �����س��� ��
	static bool bTest =false;

	if(bTest)	m_fReaction=0.001f;
	else		m_fReaction=0;
	bTest = !bTest;

	m_pGun->Translation(0.008f,-0.008f,0.015f+m_fReaction);
	m_pGunBarrel->Translation(0.008f,-0.008f,0.023f+m_fReaction);

	m_pFireTimer->Begin();

	// ����Ʈ ����~~

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
				tagInfo.vPos = m_pPick->m_vIntersectPt;		// ����Ʈ ���� ����
				
				// ��� ���ϱ�
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
		default: return false;	// ����Ʈ ����
		}

		(*iter)->SetEffect(tagInfo);
	}

	return true;
}

bool CPlayer::Collision(CPicking *pPick)
{
	m_pPick = pPick;	// ��ŷ�� ���� (������ �Ǵ� ����)�� ���� ������ ������ �ִ�.
	return true;
}
