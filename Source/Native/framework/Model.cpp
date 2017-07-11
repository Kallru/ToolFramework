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

// �� �Լ����� ���� �αװ����� ������ �ʴ´�.
// �Լ� �ȿ��� ȣ��Ǵ� �ٸ� �Լ��鿡�� ������ �°� �αװ����� ����� �����̴�.
// �ٸ� Ŭ���� �ۿ��� �� �Լ��� ����/���� �ߴٴ� ��Ǹ� �˷��ش�.
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
	//	1. ������ �д´�.
	//	2. ���Ͽ� �ִ� �޽��̸��� ���� �޽��� �ε� �Ǿ����� ã�´�.
	//	3. ���Ͽ� �ִ� �ؽ�ó�̸��� "�������ϰ��\\�ؽ�ó�̸�"���� �����Ѵ�.
	//	4. ã�� �޽��� �����ͷ� �ؽ�ó�� �ε��ϴ°ɷ� �� �Լ��� ���� ������.
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

	// ��~~~���ϴ�
	// ������ ���鼭 �Ž� �̸��ϰ� �ؽ�ó �̸��� �ɷ�����.
	// ~~_default.skin�� ���� ��ο��� �ؽ�ó�� �о���δ�. (�ڱ� �ڽ��� skin����)
	char strTexture[128]={NULL,};
		
	char *p=strtok(strBuffer,",\r\n");
	CMesh *pMesh=NULL;
	
	while(p)
	{		
		while (!pMesh && p)
		{
			pMesh = GetMesh(p);		// �Ž��� ã�ƺ���.
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
		
		// ������\\��_default.skin -> ������\\�ؽ�ó�̸�
		// ��ȯ������ �ؾߵ�.
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
	D3DXMatrixRotationQuaternion(&matRotate,&m_quateRotate);	// ��Ʈ������

	// ī�޶� ���� ���̱� ���ؼ�
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

	// ���� ������ ������ �ҽ��� �����������°հ� -_-!

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
		OutputDebugString("�����ڵ����\n");
		return E_FAIL;
	}

	DWORD dwSize;

	//---------------------------------------------------------------------------------------------------------------
	// �⺻���� MD3 ���
	ReadFile(m_FileHandle,&m_MD3Header,sizeof(MD3HEADER),&dwSize,NULL);

	//---------------------------------------------------------------------------------------------------------------
	// BoneFrame. ������ŭ ���� �� �� ������ ��ŭ ������.
	m_pBoundBox = new MD3BOUNDBOX[ m_MD3Header.iBoneFrameNum ];
	ReadFile(m_FileHandle,m_pBoundBox,sizeof(MD3BOUNDBOX)*m_MD3Header.iBoneFrameNum,&dwSize,NULL);

	//---------------------------------------------------------------------------------------------------------------
	// Tag ��...������ ���� ���ϰڴ�. �׷��� �� ����� ��
	// ��ķ� �Ǿ� �ִ���...�ƴ� ���� �Ǿ� �ִ°� -_-;
	// ���� ����� ������ �� ���� �ϳ�.
	MD3TAG *pTag = new MD3TAG[ m_MD3Header.iTagNum * m_MD3Header.iBoneFrameNum ];
	ReadFile(m_FileHandle,pTag, sizeof(MD3TAG)*m_MD3Header.iTagNum*m_MD3Header.iBoneFrameNum,&dwSize,NULL );

	m_pTag = new D3DTAG[ m_MD3Header.iTagNum * m_MD3Header.iBoneFrameNum ];

	// Matrix�� �ھ� �־��ش�.
	for(int i=0; i< m_MD3Header.iTagNum * m_MD3Header.iBoneFrameNum; ++i )
	{
		strcpy( &m_pTag[i].szTagName[0], &pTag[i].szTagName[0]);

		// 3x3 �迭�� ���鼭 m_pTag ��Ʈ������ �ִ´�.
		for(int j=0; j<3; j++)
		{
			int n;
			for(n=0; n<3; n++)
				m_pTag[i].matTag(j,n) = pTag[i].fMatrix[j][n];
			m_pTag[i].matTag(j,n) = 0.0f; // ������ 3�� �迭�� 0����
		}

		int l;
		for(l=0; l<3; l++)
			m_pTag[i].matTag(3,l) = pTag[i].Position[l];
		m_pTag[i].matTag(3,l) = 1.0f;	// ������ �迭�� 1.0����~
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
		m_pMesh[i].SetDevice(m_pd3dDevice); // ���� ���� ���ϴ� -_-;; �ϴ� �׳� ����. ����å�� ������??
		m_pMesh[i].Load(m_FileHandle); // m_FileHandle�� ���۷�����
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
