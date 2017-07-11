// File. Effect.h

// �ϴ� ���� �����ϰ� ������.

#ifndef _EFFECT_
#define _EFFECT_

enum EffectType { ET_Unkown = 0, ET_FireBroken, ET_Firegun };	// ���ڰ� ET�� ��� ��

typedef struct EFFECT_INFO
{
	D3DXVECTOR3 vDir;
	D3DXVECTOR3 vPos;

	DWORD dwCurTexFrame;
	DWORD dwCurMeshFrame;

	bool bActive;

	CTimer m_Timer;

	EFFECT_INFO():vDir(0,0,0),vPos(0,0,0),dwCurTexFrame(0),
	dwCurMeshFrame(0),bActive(false)
	{}
}EFFECT_INFO;

// ����Ʈ ���� ������ ���� �� ���� ( �ᱹ �ϵ��ڵ� OTL )
typedef struct EFFECT_CREATEINFO
{
	LPSTR lpFileName;	// ���� �̸� (�� �Ǵ� �޽�)
	DWORD dwFileNum;	// ���� ���� (�ϴ� �Ѱ���)

	LPSTR lpTexName;	// �ؽ�ó �̸�
	DWORD dwTexNum;	// �ؽ�ó ����

	DWORD dwLifeTime;	// ����Ʈ�� ������Ÿ�� (�������� �ð���)
	
	EFFECT_CREATEINFO():
	lpFileName(NULL),dwFileNum(0),lpTexName(NULL),
	dwTexNum(0),dwLifeTime(0)
	{}

}EFFECT_CREATEINFO;

class CEffect
{
public:
	CEffect();
	virtual ~CEffect();

	//bool	GetActive() { return m_bActive; }

	virtual HRESULT Create(const EFFECT_CREATEINFO& tagCreate) = 0;
	virtual bool SetEffect(const EFFECT_INFO& tagInfo) = 0;

	virtual bool FrameMove()=0;
	virtual bool Render()=0;
	virtual bool Scaling(float x,float y,float z) = 0;
	
	EffectType GetType() { return m_Type; }
	
protected:
	LPDIRECT3DDEVICE9	m_pd3dDevice;

	EffectType			m_Type;

	list<EFFECT_INFO*>	m_listInfo;
};

#endif