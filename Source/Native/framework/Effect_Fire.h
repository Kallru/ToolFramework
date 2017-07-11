// Effect_Fire.h: interface for the CEffect_Fire class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_EFFECT_FIRE_H__F9BE2E56_B436_4C69_B09C_8B8DBA75FD0B__INCLUDED_)
#define AFX_EFFECT_FIRE_H__F9BE2E56_B436_4C69_B09C_8B8DBA75FD0B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CEffect_Fire  : public CEffect
{
public:
	CEffect_Fire(LPDIRECT3DDEVICE9 pDevice,CCamera *pCamera);
	virtual ~CEffect_Fire();

	virtual HRESULT Create(const EFFECT_CREATEINFO& tagCreate);
	virtual bool SetEffect(const EFFECT_INFO& tagInfo);

	virtual bool FrameMove();
	virtual bool Render();
	virtual bool Scaling(float x,float y,float z)	{ m_pModel->SetScaling(x,y,z); return true; }

private:
	LPDIRECT3DTEXTURE9 m_pTex;
	CXFile	* m_pModel;

	EFFECT_CREATEINFO m_tagCreateInfo;
	CCamera *m_pCamera;
};

#endif // !defined(AFX_EFFECT_FIRE_H__F9BE2E56_B436_4C69_B09C_8B8DBA75FD0B__INCLUDED_)
