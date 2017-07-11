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
#define MODEL_TYPEMAX 4 // 모델타입의 최대치를 정한다~

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
	// 왜 3개씩 일까 -> x,y,z
	D3DXVECTOR3 vMins;		// 바운딩 박스의 첫번째 모퉁이,  / 원문(first corner of the bounding box)
	D3DXVECTOR3 vMaxs;		// 바운딩 박스의 두번째 모퉁이,  / 원문(second corner of the bounding box)
	D3DXVECTOR3 Position;		// 바운딩박스의 중심좌표(origin) / 원문(position/origin of the bounding box)
	float scale;			// 바운딩구의 라디우스값?		 / 원문(radius of bounding sphere)
	char name[16];			// 신경쓸필요 없음.
}MD3BOUNDBOX;

//----------------------------------------------------------------------------------//
// 파일에서 읽어올때만 쓰고 버린다~
typedef struct 
{
	char	szTagName[64];
	float	Position[3];	// 태그의 위치인가
	float	fMatrix[3][3]; // 태그가 가리키는 방향
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
// 스크립트 관련 파일을 읽는 구조체라는데...
// 아래 2개 구조체는 (texname,triangle) 파일을 읽어올때만 쓴다.
typedef struct 
{
	char szString[68];
}MD3TEXNAME;

typedef struct 
{
	int Index[3];
}MD3TRIANGLE;

//-----------------------------------------------------------------------------------//
// 뽀텍스 설명~
// 각각을 압축해서 저장하는데, 밑에 각 압축 방식
//-----------------------------------------------------------------------------------//
// 뽀텍스 : 스케일링값을 가지고 압축한다.
// float형 좌표 * 스켈링 = 최종값 | 디코딩할때는 반대로 short형 좌표 / 스켈링 = 최종값
// 법선 : 원래라면 flaot x,y,z가 필요한데 short형으로 압축함
// 뭐 위도와 경도가 어쩌고 해서 x,y 두개로 표현이 가능한데 각각 값을 0~255로 제한 시키면
// 각각을 8비트에 담을수 있으니, 총 16비트해서 short 한개에 표현가능하다고 함 ㅡㅡ;
typedef struct 
{
	short Vertex[3];
	short Normal;
}MD3VERTEX_FILE;

//------------------------------------------------------------------------------------//
// 아래 두 구조체는 실제로 랜더링 하거나 할때 쓰는 구조체
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
	//	중요한 함수들
	//-------------------------------------------------------------------------//
	// 생성 편의를 위해서 한개의 함수로 통합~
	HRESULT CreateModel(char *szMD3FileName, char *szTexFileName);
	
	HRESULT LoadMD3File(char *szFileName);
	HRESULT LoadTexture(char *szFileName);
	HRESULT CreateVertexIndexBuffer();

	void Render();
	void Release();

	void BoundingBoxRender();

	//--------------------------------------------------------------------------//
	//	도움을 주는 메소드들(??)
	//--------------------------------------------------------------------------//
	void		RotateAxis(D3DXVECTOR3 *vAxis,float Angle);
	// 편의상 오버로딩 해둔다
	void		Scaling(float x,float y, float z) { this->Scaling(D3DXVECTOR3(x,y,z)); }
	void		Scaling(const D3DXVECTOR3 &vPos) { D3DXMatrixScaling(&m_matScale,vPos.x,vPos.y,vPos.z); }
	void		Translation(float x,float y,float z) { this->Translation(D3DXVECTOR3(x,y,z)); }
	void		Translation(const D3DXVECTOR3 &vPos) { D3DXMatrixTranslation(&m_matTrans,vPos.x,vPos.y,vPos.z); }

	// 랜더 적용 및 카메라가 필요해서..
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

	CCamera *m_pCam;	// 카툰랜더 때문에 필요, 내용을 수정하거나 하면 안됨..

	int m_nType;
	float m_finterPolation; // 보간에 쓰이는거

	HANDLE m_FileHandle;
	
	CMesh *m_pMesh;
	
	// MD3
	MD3HEADER m_MD3Header;
	D3DTAG *m_pTag;
	MD3BOUNDBOX *m_pBoundBox;

	// Animation 변수
	vector<MD3ANI> m_listAnimation;
	UINT m_nStartFrame;
	UINT m_nEndFrame;

	UINT m_nCurFrame;
	UINT m_nNextFrame;
	DWORD m_dwMSPF;		// Mill Second Per Frame
	DWORD m_dwAccTime;	// 누적~
};

#endif