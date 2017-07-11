// Model.cpp
#include <d3d9.h>
#include <d3dx9.h>
#include "Model.h"
#include "Mesh.h"
#include "Camera.h"

CModel::CModel(LPDIRECT3DDEVICE9 pd3dDevice)
:m_pd3dDevice(pd3dDevice),m_finterPolation(0.0f),m_nStartFrame(0),m_nEndFrame(0),m_nCurFrame(0),
m_nNextFrame(0),m_dwMSPF(0),m_dwAccTime(0)
{
	D3DXMatrixIdentity(&m_matWorld);
	D3DXMatrixIdentity(&m_matTrans);
	D3DXMatrixIdentity(&m_matScale);
	D3DXQuaternionIdentity(&m_quateRotate);
}

CModel::~CModel()
{
	Release();
}

void CModel::Release()
{
	SAFE_DELETE_ARRAY( m_pBoundBox );
	SAFE_DELETE_ARRAY(m_pTag);
	SAFE_DELETE_ARRAY(m_pMesh);
}

// 이 함수에선 따로 로그같은걸 남기지 않는다.
// 함수 안에서 호출되는 다른 함수들에서 각각에 맞게 로그같은걸 남기기 때문이다.
// 다만 클래스 밖에서 이 함수가 성공/실패 했다는 사실만 알려준다.
HRESULT CModel::CreateModel(char *szMD3FileName, char *szTexFileName)
{
	//----------------------------------//
	//	MD3File Load!!
	//----------------------------------//
	if( FAILED( LoadMD3File(szMD3FileName) )) 
		return E_FAIL;
	//----------------------------------//
	// Texture Load!!
	//----------------------------------//
	if( FAILED( LoadTexture(szTexFileName) ))
		return E_FAIL;
	//----------------------------------//
	// CreateVertex & CreateIndex
	//----------------------------------//
	if( FAILED( CreateVertexIndexBuffer() ))
		return E_FAIL;

	return S_OK;
}

HRESULT CModel::LoadTexture(char *szFileName)
{
	//----------------------------------------------------------------------//
	//	1. 파일을 읽는다.
	//	2. 파일에 있는 메쉬이름과 같은 메쉬가 로드 되었는지 찾는다.
	//	3. 파일에 있는 텍스처이름을 "현재파일경로\\텍스처이름"으로 셋팅한다.
	//	4. 찾은 메쉬의 포인터로 텍스처를 로딩하는걸로 이 함수의 일을 끝낸다.
	//----------------------------------------------------------------------//
	//////////////////////////////////////////////////////////////////////////
	HANDLE hRpt = CreateFile(szFileName,
							GENERIC_READ, FILE_SHARE_READ,
							NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if(!hRpt)
	{
		OutputDebugString("[failed] CModel::Loadtexture / CreaetFile \n");
		return E_FAIL;
	}

	DWORD dwSize=GetFileSize(hRpt,NULL);

	char *strBuffer = new char[dwSize+1];
	ZeroMemory(strBuffer,dwSize+1);
	ReadFile(hRpt,strBuffer,dwSize,&dwSize,NULL);
	CloseHandle(hRpt);

	// 복~~~잡하다
	// 루프를 돌면서 매쉬 이름하고 텍스처 이름만 걸러낸다.
	// ~~_default.skin와 같은 경로에서 텍스처를 읽어들인다. (자기 자신의 skin파일)
	char strTexture[128]={NULL,};
		
	char *p=strtok(strBuffer,",\r\n");
	CMesh *pMesh=NULL;
	
	while(p)
	{		
		while (!pMesh && p)
		{
			pMesh = GetMesh(p);		// 매쉬를 찾아본다.
			p=strtok(NULL,",\r\n");
		}

		if(pMesh)
		{ 
			if(SUCCEEDED(pMesh->LoadTexture(p)))
			{
				pMesh=NULL;
				continue;
			}
		}
		else if(NULL==p) break;

		strcpy(&strTexture[0],szFileName);
		
		// 현재경로\\∼_default.skin -> 현재경로\\텍스처이름
		// 변환가능을 해야됨.
		int nSize=strlen(szFileName);
		for(int i=nSize; i>0; i--)
		{
			if(szFileName[i-1]=='\\')
			{
				ZeroMemory(&strTexture[i],nSize-i);
				sprintf(&strTexture[i],"%s",p);
				break;
			}
		}
			
		if(pMesh)
		{
			pMesh->LoadTexture(&strTexture[0]);
		}
		pMesh=NULL;
	}

	return S_OK;
}

CMesh *CModel::GetMesh(char *pBuffer)
{
	for(int i=0; i<m_MD3Header.iMeshNum; i++)
	{
		if(!strcmp(pBuffer,m_pMesh[i].m_MeshHeader.szName) )
		{
			return &m_pMesh[i];
		}
	}
	return NULL;
}

void CModel::RotateAxis(D3DXVECTOR3 *pvAxis,float Angle)
{
	D3DXQuaternionRotationAxis(&m_quateRotate,pvAxis,Angle);
}

void CModel::SetWorldMat()
{
	D3DXMATRIX matRotate;
	D3DXMatrixRotationQuaternion(&matRotate,&m_quateRotate);	// 매트릭스로

	// 카메라에 모델을 붙이기 위해서
	D3DXMATRIX matInView = *m_pCam->GetViewMatrix();
	D3DXMatrixInverse(&matInView, NULL, &matInView);

	m_matWorld = m_matScale * matRotate * m_matTrans * matInView;
}

void CModel::Render()
{	
	this->SetWorldMat();
	m_pd3dDevice->SetTransform(D3DTS_WORLD,&m_matWorld);
	
	// Render State
	m_pd3dDevice->SetRenderState( D3DRS_LIGHTING,		TRUE);
	m_pd3dDevice->SetRenderState( D3DRS_ZENABLE,		TRUE );
	m_pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE,	TRUE );
	m_pd3dDevice->SetRenderState( D3DRS_FOGENABLE,		TRUE );
	m_pd3dDevice->SetRenderState( D3DRS_CULLMODE,		D3DCULL_CW );

	m_pd3dDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
	m_pd3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );

	for(int i=0; i<m_MD3Header.iMeshNum; i++)
	{
		if(FAILED( m_pMesh[i].Render(0,0,0) ))
			return;
	}

	//m_pd3dDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);

	/*

	// 괘니 툰랜더 때문에 소스만 지저분해지는겐가 -_-!

	D3DXMATRIX matView = m_pCam->GetViewMatrix();
	D3DXMATRIX matProj = m_pCam->GetProjMatrix();

	D3DXMATRIX matShader = m_matWorld * matView *  matProj;

	D3DXMatrixTranspose(&matShader, &matShader);
	m_pd3dDevice->SetVertexShaderConstantF(0, (float *)&matShader, 4);

	D3DXVECTOR4 LightDir(0.0, -1.0f, 0.0f, 0.0f);
	D3DXMATRIX InverseWorld;

	D3DXMatrixInverse(&InverseWorld, NULL, &m_matWorld);
	D3DXVec4Transform(&LightDir, &LightDir, &InverseWorld);

	D3DXVECTOR4 vEyePos = m_pCam->GetEyePt();
	D3DXVec4Transform(&vEyePos, &vEyePos, &InverseWorld);

	m_pd3dDevice->SetVertexShaderConstantF(5, (float *)&LightDir, 1);
	m_pd3dDevice->SetVertexShaderConstantF(6, (float *)&vEyePos, 1);

	/*

	m_pd3dDevice->SetRenderState( D3DRS_DITHERENABLE,   FALSE );
    m_pd3dDevice->SetRenderState( D3DRS_SPECULARENABLE, FALSE );
    m_pd3dDevice->SetRenderState( D3DRS_ZENABLE,        TRUE );
    m_pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );

	D3DXMATRIX matView = m_pCam->GetViewMatrix();
	D3DXMATRIX matProj = m_pCam->GetProjMatrix();
	
	D3DXMATRIX    trans; 
    D3DXMatrixTranspose( &trans , &(m_matWorld * matView) );
    m_pd3dDevice->SetVertexShaderConstantF( 1, (float*)&trans, 4 );

	D3DXMatrixTranspose( &trans , &(m_matWorld * matView * matProj) );
    m_pd3dDevice->SetVertexShaderConstantF( 5, (float*)&trans, 4 );
	
	// Load light direction into shader constant 
	D3DXVECTOR3    lightdir(-1.0f,-1.0f,1.0f); 
	D3DXVec3Normalize( &lightdir , &lightdir ); 
	D3DXVec3TransformNormal( &lightdir , &lightdir , &matView ); 
	float    ld[4] = {lightdir.x,lightdir.y,lightdir.z,0}; 
	m_pd3dDevice->SetVertexShaderConstantF( 10, ld , 1 ); 
	
	D3DXVECTOR4 C0 = D3DXVECTOR4( 0.0f, 0.5f, 1.0f, 2.0f );
	m_pd3dDevice->SetVertexShaderConstantF( 0, C0 , 1 );

	m_pMesh[0].Render(0,0,0);

	/**/
}

HRESULT CModel::LoadMD3File(char *szFileName)
{
	m_FileHandle = CreateFile(szFileName,GENERIC_READ,0,NULL,
					OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
	if(!m_FileHandle)
	{
		OutputDebugString("파일핸들실패\n");
		return E_FAIL;
	}

	DWORD dwSize;

	//---------------------------------------------------------------------------------------------------------------
	// 기본적인 MD3 헤더
	ReadFile(m_FileHandle,&m_MD3Header,sizeof(MD3HEADER),&dwSize,NULL);

	//---------------------------------------------------------------------------------------------------------------
	// BoneFrame. 개수만큼 생성 후 그 사이즈 만큼 읽으삼.
	m_pBoundBox = new MD3BOUNDBOX[ m_MD3Header.iBoneFrameNum ];
	ReadFile(m_FileHandle,m_pBoundBox,sizeof(MD3BOUNDBOX)*m_MD3Header.iBoneFrameNum,&dwSize,NULL);

	//---------------------------------------------------------------------------------------------------------------
	// Tag 흠...예제를 이해 못하겠당. 그러니 내 맘대로 함
	// 행렬로 되어 있는지...아님 어케 되어 있는겨 -_-;
	// 값은 제대로 들어오는 것 같긴 하네.
	MD3TAG *pTag = new MD3TAG[ m_MD3Header.iTagNum * m_MD3Header.iBoneFrameNum ];
	ReadFile(m_FileHandle,pTag, sizeof(MD3TAG)*m_MD3Header.iTagNum*m_MD3Header.iBoneFrameNum,&dwSize,NULL );

	m_pTag = new D3DTAG[ m_MD3Header.iTagNum * m_MD3Header.iBoneFrameNum ];

	// Matrix에 자알 넣어준다.
	for(int i=0; i< m_MD3Header.iTagNum * m_MD3Header.iBoneFrameNum; ++i )
	{
		strcpy( &m_pTag[i].szTagName[0], &pTag[i].szTagName[0]);

		// 3x3 배열을 돌면서 m_pTag 매트릭스에 넣는다.
		for(int j=0; j<3; j++)
		{
			int n;
			for(n=0; n<3; n++)
				m_pTag[i].matTag(j,n) = pTag[i].fMatrix[j][n];
			m_pTag[i].matTag(j,n) = 0.0f; // 마지막 3번 배열은 0으로
		}

		int l;
		for(l=0; l<3; l++)
			m_pTag[i].matTag(3,l) = pTag[i].Position[l];
		m_pTag[i].matTag(3,l) = 1.0f;	// 마지막 배열은 1.0으로~
	}

	SAFE_DELETE_ARRAY(pTag);

	//=====================================//
	//	M e s h
	//=====================================//
	
	LoadMesh();
	
	CloseHandle(m_FileHandle);

	return S_OK;
}

HRESULT CModel::CreateVertexIndexBuffer()
{
	for(int i=0; i<m_MD3Header.iMeshNum; i++)
	{
		if( FAILED( m_pMesh[i].CreateVertexBuffer() ) )
			return E_FAIL;
		if( FAILED( m_pMesh[i].CreateIndexBuffer() ) )
			return E_FAIL;
	}
	return S_OK;
}

void CModel::LoadMesh()
{
	m_pMesh = new CMesh[ m_MD3Header.iMeshNum ];
	for(int i=0; i<m_MD3Header.iMeshNum; i++)
	{
		m_pMesh[i].SetDevice(m_pd3dDevice); // 낭비가 정말 심하다 -_-;; 일단 그냥 하자. 차선책이 없을까??
		m_pMesh[i].Load(m_FileHandle); // m_FileHandle는 레퍼런스임
	}
}

void CModel::BoundingBoxRender()
{
	LPD3DXMESH pMesh;
	MD3BOUNDBOX *pBox = &m_pBoundBox[m_nCurFrame];

	D3DVECTOR vSize = pBox->vMaxs - pBox->vMins;
	
	D3DXCreateBox(m_pd3dDevice,vSize.x,vSize.y,vSize.z,&pMesh,NULL);

	DWORD Temp;
	m_pd3dDevice->GetRenderState(D3DRS_ZENABLE, &Temp);
	m_pd3dDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);
	m_pd3dDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
	m_pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	D3DXMATRIXA16 matResult;
	D3DVECTOR vLastPos;

	vLastPos.x = (vSize.x/2)+(pBox->vMins.x);
	vLastPos.y = (vSize.y/2)+(pBox->vMins.y);
	vLastPos.z = (vSize.z/2)+(pBox->vMins.z);

	D3DXMatrixTranslation(&matResult,vLastPos.x,vLastPos.y,vLastPos.z);

	D3DXMatrixMultiply(&matResult, &matResult, &m_matWorld);
	m_pd3dDevice->SetTransform(D3DTS_WORLD,&matResult);
	
	pMesh->DrawSubset(0);

	m_pd3dDevice->SetRenderState(D3DRS_ZENABLE, Temp);
	m_pd3dDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
	m_pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);

	SAFE_RELEASE(pMesh);
}
