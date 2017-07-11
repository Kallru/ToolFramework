//Mesh.h

#ifndef _MESH_
#define _MESH_

class CMesh
{
public:
	CMesh();
	~CMesh();

	void Load(HANDLE &pFile);
	HRESULT LoadTexture(char *szFileName);
	HRESULT CreateVertexBuffer();
	HRESULT CreateIndexBuffer();

	HRESULT Render(UINT nCurFrame, UINT nNextFrame, float fInterPolation);
	D3DXVECTOR3 DecodeNormal(short normal);

	void SetDevice(LPDIRECT3DDEVICE9 pd3dDevice){m_pd3dDevice = pd3dDevice;}	// ³ªÁß¿¡ ¹Ù²¨¾ßµÊ.

	void Release();

	// Mesh
	MD3MESHHEADER	m_MeshHeader;
	MD3TEXNAME	*m_pTexName;
	MD3TRIANGLE *m_pTriangle;
	D3DFVFVERTEX *m_pVertex;

	LPDIRECT3DDEVICE9 m_pd3dDevice;

	LPDIRECT3DVERTEXBUFFER9 m_pVB;
	LPDIRECT3DINDEXBUFFER9 m_pIB;
	vector<LPDIRECT3DTEXTURE9> m_listTexture;
};

#endif