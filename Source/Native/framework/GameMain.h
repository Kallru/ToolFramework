//------------------------------------------------------------------------------
// File. GameMain.h
// Note. UI Ŭ������ ���� �ȸ����. (�װ� ��������� �� �����)
//------------------------------------------------------------------------------

#ifndef _GAMEMAIN_
#define _GAMEMAIN_

class CXFile;
class CCamera;
class CPlayer;
class CModel;
class ZTerrain;
class ZFrustum;
class CRender;
class CPicking;
class CDrawEx;

class CORE_API CGameMain : public CD3DApp
{
public:
	CGameMain( HWND hWnd, DWORD dwWidth, DWORD dwHegiht, const string& dataPath, int vertexProcessing );
	virtual ~CGameMain();

	virtual bool	LoadScene();
	virtual bool	FrameMove();
	virtual bool	Render();

	virtual bool	OnLButtonDown(int x,int y,UINT nFlag);
	virtual bool	OnLButtonUp(int x,int y,UINT nFlag);
	
	void	Init();
	bool	Keydown();
	bool	MouseMove();

	bool	SetupLight();
	bool	SetupPixelFog(DWORD dwColor, DWORD dwMode = D3DFOG_LINEAR);

private:
	CXFile  *	m_pSkydome;
	CCamera *	m_pCamera;
	CTimer  *	m_pTimeMove;
	CPlayer *	m_pPlayer;

	CDrawEx * m_pDraw;	// ����..UI ����� �̰� ���� �Ⱥ��̴� ���� �ռ��� - �̳༮�� �̱����ؾߵɳ�
	SPRITE_TEXTURE * m_pTarTex;	// UI�� ������ �ҽ��� ���� ����������

	ZTerrain *	m_pTerrain;
	ZFrustum *	m_pFrustum;

	POINT m_ptMouse;
	DWORD m_dwLastTime;
	DWORD m_dwCurTime;

	float m_fAspect;

	vector<CXFile*> m_listObject;

	CRender		* m_cpRenderer;	// ���� erer �ݺ��Ǵ� �̻��ϴ� ��
	CPicking	* m_pPick;
	string m_dataPath;

	bool m_mouseLock;
};

#endif