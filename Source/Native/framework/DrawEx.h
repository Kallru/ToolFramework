////////////////////////////////////////////////////////////////////////////////////////
//
//	File. CDrawEx.h
//	Note.
//	..by kallru
//
////////////////////////////////////////////////////////////////////////////////////////	

#ifndef _CDRAWEX_
#define _CDRAWEX_

struct SPRITE_VERTEX
{
	float x,y,z;
	float u,v;
};

struct SPRITE_TEXTURE
{
	LPDIRECT3DTEXTURE9 pTex;
	int m_iWidth;
	int m_iHeight;

	SPRITE_TEXTURE()
	{
		pTex = NULL;
		m_iWidth = 0;
		m_iHeight = 0;
	}

	~SPRITE_TEXTURE()
	{
		if(pTex != NULL)
			SAFE_RELEASE(pTex);
	}
};

#define D3DFVF_SPRITEVERTEX ( D3DFVF_XYZ | D3DFVF_TEX1)

class CDrawEx
{
public:
	CDrawEx();
	CDrawEx(LPDIRECT3DDEVICE9 pd3dDevice, HWND hWnd);
	~CDrawEx();
	void Release();
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-==-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
//
//	�ۿ��� ����� �Լ���
//
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-==-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
	HRESULT LoadImageFile(char *FileName, SPRITE_TEXTURE *pTexture, D3DCOLOR KeyColor=D3DCOLOR_XRGB(255,0,255));
	HRESULT InitDevice(LPDIRECT3DDEVICE9 pd3dDevice);
	HRESULT CreateSpriteBuffer();

	void DrawSpriteEx(SPRITE_TEXTURE &pTexture, float x=0.0f, float y=0.0f, DWORD dwAlpha=255,
						DWORD dwWidth=0, DWORD dwHeight=0,
						float AngleX=0.0f, float AngleY=0.0f, float AngleZ=0.0f);

	HRESULT Lock(UINT iSize,void **pData, DWORD dwFlage=0)	{ return m_pVertexBuffer->Lock(0,iSize,pData,dwFlage); }
	HRESULT Unlock()	{ return m_pVertexBuffer->Unlock(); }

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-==-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
//
//	�ȿ��� ���������, �ۿ����� ��� �� �� �ְ�...
//
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-==-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
	void DrawSprite(SPRITE_TEXTURE &pTexture, DWORD dwAlpha);
	void RotateSprite(float AngleX, float AngleY, float AngleZ, float x, float y);
	void TranslateSprite(float x, float y, float z);
	void ScaleSprite(DWORD dwWidth,DWORD dwHeight);	//���Ǹ� ���ؼ� DWORD
		
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-==-=-==-=-==-=-=-=-=-=-==-=-==-=-=-=-=-=-=-=-=-
//
//	��Ÿ �ʿ��� ��ƿ~?
//
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-==-=-==-=-==-=-=-=-=-=-==-=-==-=-=-=-=-=-=-=-=-
	// ���İ� ��� : dwColor �μ��� D3DCOLOR_ARGB( A, R, G ,B ); �� �ָ� ��.
	void CreateFillRectangle(SPRITE_TEXTURE *pOut, int nWidth, int nHeight, DWORD dwColor);
	void CreateFillRectangleEx(SPRITE_TEXTURE *pOut, int nWidth, int nHeight, DWORD dwColor, UINT MipLevels);	//  �ӷ��� ���
	void FillRectangle(int nX, int nY, int nWidth, int nHeight, DWORD dwColor=0xffffffff, float z=0.0f, float fAngle=0.0f);
	
private:

	HWND m_hWnd;
	LPDIRECT3DDEVICE9 m_pd3dDevice;
	LPDIRECT3DVERTEXBUFFER9 m_pVertexBuffer;

	D3DXMATRIXA16 m_Rotation;
	D3DXMATRIXA16 m_Translation;
	D3DXMATRIXA16 m_Scaling;

	// �������� ��� 2��
	//D3DXMATRIXA16 m_Proportion;	// ���� ����
	D3DXMATRIXA16 m_Pixel;		// �ȼ� ����

	D3DXMATRIXA16 m_ViewMatrix; // �����

};

#endif