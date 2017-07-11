////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
// 
//	Model.h
//

#ifndef _MODEL_
#define _MODEL_

//----------------------------------------------------------------------------------//
//	I n c l u d e
//----------------------------------------------------------------------------------//
#include <d3dx9.h>
#include <vector>

using namespace std;

//----------------------------------------------------------------------------------//
//	D e f i n e
//----------------------------------------------------------------------------------//
#ifndef SAFE_DELETE
#define SAFE_DELETE(p)  { if(p) { delete (p);     (p)=NULL; } }
#endif
#ifndef SAFE_DELETE_ARRAY
#define SAFE_DELETE_ARRAY(p) { if(p) {delete [](p); (p)=NULL;} }
#endif
#ifndef SAFE_RELEASE
#define SAFE_RELEASE(p) { if(p) { (p)->Release(); (p)=NULL;} }
#endif

#define MD3VERSION	15
#define MODEL_TYPEMAX 4 // ��Ÿ���� �ִ�ġ�� ���Ѵ�~

#define D3DFVF_VERTEX (D3DFVF_XYZ | D3DFVF_NORMAL|D3DFVF_TEX1 )

//----------------------------------------------------------------------------------//
//	E n u m
//----------------------------------------------------------------------------------//
// Model Type
enum
{
	MODEL_HEAD = 0,
	MODEL_UPPER,
	MODEL_LOWER,
	MODEL_WEAPON,
};

// Animation Enum
enum
{
	// Both
	BOTH_DEATH1 = 0, BOTH_DEAD1, BOTH_DEATH2, BOTH_DEAD2, BOTH_DEATH3, BOTH_DEAD3,

	// Torso
	TORSO_GESTURE, TORSO_ATTACK, TORSO_ATTACK2, TORSO_DROP, TORSO_RAISE, TORSO_STAND, TORSO_STAND2,

	// Legs
	LEGS_WALKCR, LEGS_WALK, LEGS_RUN, LEGS_BACK, LEGS_SWIM, LEGS_JUMP, LEGS_LAND, LEGS_JUMPB, LEGS_LANDB, LEGS_IDLE,
	LEGS_IDLECR, LEGS_TURN,

	MAX_ANIMATION,

	STOP_ANIMATION,
};

//----------------------------------------------------------------------------------//
//	S t r u c t
//----------------------------------------------------------------------------------//
// md3 Header
typedef struct
{
	DWORD id;
	int iVersion;
	char cFileName[68];

	int iBoneFrameNum;
	int iTagNum;
	int iMeshNum;
	int iMaxTextureNum;
	int iHeaderSize;
	
	int iTagStart;
	int iMeshStart;
	int iFileSize;
}MD3HEADER;

//----------------------------------------------------------------------------------//
// md3 BoundBox
typedef struct 
{
	// �� 3���� �ϱ� -> x,y,z
	D3DXVECTOR3 vMins;		// �ٿ�� �ڽ��� ù��° ������,  / ����(first corner of the bounding box)
	D3DXVECTOR3 vMaxs;		// �ٿ�� �ڽ��� �ι�° ������,  / ����(second corner of the bounding box)
	D3DXVECTOR3 Position;		// �ٿ���ڽ��� �߽���ǥ(origin) / ����(position/origin of the bounding box)
	float scale;			// �ٿ������ ���콺��?		 / ����(radius of bounding sphere)
	char name[16];			// �Ű澵�ʿ� ����.
}MD3BOUNDBOX;

//----------------------------------------------------------------------------------//
// ���Ͽ��� �о�ö��� ���� ������~
typedef struct 
{
	char	szTagName[64];
	float	Position[3];	// �±��� ��ġ�ΰ�
	float	fMatrix[3][3]; // �±װ� ����Ű�� ����
}MD3TAG;

//-----------------------------------------------------------------------------------//
// MD3 MeshHeader
typedef struct 
{
	DWORD id;
	char szName[68];
	int iMeshFrameNum;
	int iTextureNum;
	int iVertexNum;

	int iTriangleNum;
	int iTriangleStart;
	int iHeaderSize;
	int iTecVecStart;
	int iVertexStart;
	int iMeshSize;
}MD3MESHHEADER;

//-----------------------------------------------------------------------------------//
// ��ũ��Ʈ ���� ������ �д� ����ü��µ�...
// �Ʒ� 2�� ����ü�� (texname,triangle) ������ �о�ö��� ����.
typedef struct 
{
	char szString[68];
}MD3TEXNAME;

typedef struct 
{
	int Index[3];
}MD3TRIANGLE;

//-----------------------------------------------------------------------------------//
// ���ؽ� ����~
// ������ �����ؼ� �����ϴµ�, �ؿ� �� ���� ���
//-----------------------------------------------------------------------------------//
// ���ؽ� : �����ϸ����� ������ �����Ѵ�.
// float�� ��ǥ * ���̸� = ������ | ���ڵ��Ҷ��� �ݴ�� short�� ��ǥ / ���̸� = ������
// ���� : ������� flaot x,y,z�� �ʿ��ѵ� short������ ������
// �� ������ �浵�� ��¼�� �ؼ� x,y �ΰ��� ǥ���� �����ѵ� ���� ���� 0~255�� ���� ��Ű��
// ������ 8��Ʈ�� ������ ������, �� 16��Ʈ�ؼ� short �Ѱ��� ǥ�������ϴٰ� �� �Ѥ�;
typedef struct 
{
	short Vertex[3];
	short Normal;
}MD3VERTEX_FILE;

//------------------------------------------------------------------------------------//
// �Ʒ� �� ����ü�� ������ ������ �ϰų� �Ҷ� ���� ����ü
typedef struct 
{
	D3DXVECTOR3 vVector;
	D3DXVECTOR3 nNoraml;
	D3DXVECTOR2 Tex;
}D3DFVFVERTEX;

typedef struct 
{
	char szTagName[64];
	D3DXMATRIXA16 matTag;
}D3DTAG;

// Animation
typedef struct 
{
	UINT nType;
	UINT nFirstFrame, nNumFrames,nLoopingFrames, nFramesPerSecond;
}MD3ANI;

class CMesh;
class CCamera;

class CModel
{
public:

	CModel(LPDIRECT3DDEVICE9 pd3dDevice);
	~CModel();

	//-------------------------------------------------------------------------//
	//	�߿��� �Լ���
	//-------------------------------------------------------------------------//
	// ���� ���Ǹ� ���ؼ� �Ѱ��� �Լ��� ����~
	HRESULT CreateModel(char *szMD3FileName, char *szTexFileName);
	
	HRESULT LoadMD3File(char *szFileName);
	HRESULT LoadTexture(char *szFileName);
	HRESULT CreateVertexIndexBuffer();

	void Render();
	void Release();

	void BoundingBoxRender();

	//--------------------------------------------------------------------------//
	//	������ �ִ� �޼ҵ��(??)
	//--------------------------------------------------------------------------//
	void		RotateAxis(D3DXVECTOR3 *vAxis,float Angle);
	// ���ǻ� �����ε� �صд�
	void		Scaling(float x,float y, float z) { this->Scaling(D3DXVECTOR3(x,y,z)); }
	void		Scaling(const D3DXVECTOR3 &vPos) { D3DXMatrixScaling(&m_matScale,vPos.x,vPos.y,vPos.z); }
	void		Translation(float x,float y,float z) { this->Translation(D3DXVECTOR3(x,y,z)); }
	void		Translation(const D3DXVECTOR3 &vPos) { D3DXMatrixTranslation(&m_matTrans,vPos.x,vPos.y,vPos.z); }

	// ���� ���� �� ī�޶� �ʿ��ؼ�..
	void		SetCamera(CCamera *pCam) { m_pCam=pCam; }
	//void		SetViewProj(const D3DXMATRIXA16& mat,const D3DXMATRIXA16& pro) { m_matView = mat; m_matProj = pro;}

	void SetWorldMat();
	D3DXMATRIX GetWorldMat() { return m_matWorld; }

	CMesh *GetMesh(char*pBuffer);
	void SetType(int nType) { m_nType = nType; }
	int GetType(){ return m_nType; }

private:
	void LoadMesh();

private:
	LPDIRECT3DDEVICE9 m_pd3dDevice;

	D3DXMATRIXA16 m_matWorld;
	D3DXMATRIXA16 m_matTrans;
	D3DXMATRIXA16 m_matScale;
	D3DXQUATERNION m_quateRotate;

	CCamera *m_pCam;	// ī������ ������ �ʿ�, ������ �����ϰų� �ϸ� �ȵ�..

	int m_nType;
	float m_finterPolation; // ������ ���̴°�

	HANDLE m_FileHandle;
	
	CMesh *m_pMesh;
	
	// MD3
	MD3HEADER m_MD3Header;
	D3DTAG *m_pTag;
	MD3BOUNDBOX *m_pBoundBox;

	// Animation ����
	vector<MD3ANI> m_listAnimation;
	UINT m_nStartFrame;
	UINT m_nEndFrame;

	UINT m_nCurFrame;
	UINT m_nNextFrame;
	DWORD m_dwMSPF;		// Mill Second Per Frame
	DWORD m_dwAccTime;	// ����~
};

#endif