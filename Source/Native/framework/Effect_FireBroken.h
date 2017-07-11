//---------------------------------------------------------------------------
// File. Effect_FireBroken.h
// Note. 메쉬 개념은 안둔다. 게을러스 -_-ㅋㅋ
//---------------------------------------------------------------------------

#ifndef _EFFECT_FIREBROKEN_
#define _EFFECT_FIREBROKEN_

class CXFile;
class CTimer;

typedef struct 
{
	DWORD dwIndex;	// 의미는 없을지도..
	LPDIRECT3DTEXTURE9 pTex;
	DWORD dwMillSec;
}SPRITE;

class CEffect_FireBroken : public CEffect
{
public:
	CEffect_FireBroken(LPDIRECT3DDEVICE9 pDevice);
	virtual ~CEffect_FireBroken();

	virtual HRESULT Create(const EFFECT_CREATEINFO& tagCreate);

	HRESULT AddTexture(LPSTR lpFileName, DWORD dwMillSecTime);
	HRESULT AddTexture(SPRITE *pSprite);

	virtual bool FrameMove();
	virtual bool Render();
	virtual bool SetEffect(const EFFECT_INFO& tagInfo);
	virtual bool Scaling(float x,float y,float z);

	//LPDIRECT3DTEXTURE9 GetCurTexture(LPDIRECT3DTEXTURE9 pTex=NULL);
	//{ return (pTex) ? pTex = m_listSprite[m_dwCurTexFrame]->pTex : m_listSprite[m_dwCurTexFrame]->pTex; }

	//SPRITE * GetCurTexture(SPRITE *pTex=NULL)
	//{ return (pTex) ? pTex = m_listSprite[m_dwCurTexFrame] : m_listSprite[m_dwCurTexFrame]; }
	
protected:
	vector<SPRITE *> m_listSprite;

	vector<CXFile *> m_listModel;
	//CTimer * m_pTimer;
};
#endif