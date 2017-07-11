// File. Effect.h

// 일단 가장 간단하게 만들자.

#ifndef _EFFECT_
#define _EFFECT_

enum EffectType { ET_Unkown = 0, ET_FireBroken, ET_Firegun };	// 약자가 ET가 됬네 ㅋ

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

// 이펙트 툴이 없으니 ㅋㅋ 걍 수동 ( 결국 하드코딩 OTL )
typedef struct EFFECT_CREATEINFO
{
	LPSTR lpFileName;	// 파일 이름 (모델 또는 메쉬)
	DWORD dwFileNum;	// 파일 개수 (일단 한개임)

	LPSTR lpTexName;	// 텍스처 이름
	DWORD dwTexNum;	// 텍스처 개수

	DWORD dwLifeTime;	// 이팩트의 라이프타임 (보여지는 시간임)
	
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