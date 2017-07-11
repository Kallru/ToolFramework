//------------------------------------------------------------------------------
// File. Player.h
//------------------------------------------------------------------------------

#ifndef _PLAYER_
#define _PLAYER_

class CPicking;
class CEffect;

enum PlayerState { PS_Unkown = 0, PS_FireStart, PS_FireStop };

class CPlayer
{
public:
	CPlayer();
	~CPlayer();

	HRESULT Create(LPDIRECT3DDEVICE9 pD3DDevice, CCamera *pCamera, const D3DXVECTOR3& vStartPos,ZTerrain * pTerrain);

	/*bool MoveZ(char szType)
	{
		if( '+' == szType )	m_fSpeedZ += 2;
		else m_fSpeedZ -= 2;
		return this->Move(szType,m_vLook);
	}
	bool MoveX(char szType)
	{
		if( '+' == szType )	m_fSpeedX += 2;
		else m_fSpeedX -= 2;
		return this->Move(szType,m_vRight); 
	}
	bool MoveY(char szType){ return this->Move(szType,m_vUpPt); }
	*/

	bool Move(char szKey);

	bool RotateX(float fAngle){ return this->Roate('x',fAngle); }
	bool RotateY(float fAngle){ return this->Roate('y',fAngle); }

	bool Render();
	bool SetLight();

	bool Collision(CPicking *pPick);

	bool Act_Attack();

	bool State(PlayerState state);

	bool FrameMove(float FrameDelta);

	void		SetPos(D3DXVECTOR3 vPos);
	D3DXVECTOR3 GetPos()	{ return m_vPos; }
	
private:
	bool Move(char szType, D3DXVECTOR3 vDir);
	bool Roate(char szType, float fAngle);

protected:
	D3DXVECTOR3 m_vPos;
	D3DXVECTOR3 m_vLook;
	D3DXVECTOR3 m_vUpPt;
	D3DXVECTOR3 m_vRight;

	CTimer	*m_pMoveTimer;
	CTimer	*m_pFireTimer;
	CCamera *m_pCamera;

	CModel *m_pGun;
	CModel *m_pGunBarrel;

	float m_fCamHeight;
	float m_fMoveSpeed;

	float m_fMapHeight;	// 계산량을 한번 줄이기 위해

	LPDIRECT3DDEVICE9 m_pD3DDevice;

	CPicking *m_pPick;
	ZTerrain *m_pTerrain;

	vector<CEffect *> m_pFireEffect;

	bool	m_bFire; // 공격중인가
	float	m_fReaction;	// 반동의 정도
};

#endif