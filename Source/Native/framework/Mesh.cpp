#include "Model.h"
#include "Mesh.h"

CMesh::CMesh()
{
}

CMesh::~CMesh()
{
	Release();
}

void CMesh::Release()
{
	SAFE_DELETE_ARRAY(m_pTexName);
	SAFE_DELETE_ARRAY(m_pTriangle);
	SAFE_DELETE_ARRAY(m_pVertex);
	m_listTexture.clear();

	SAFE_RELEASE(m_pVB);
	SAFE_RELEASE(m_pIB);
}

//----------------------------------------------------------------------------------------//
// ������������ �����غ���. 
// ��ü������ ����(����/�浵)�� ǥ���ϸ� �װ��� �������� ������ �����Ѽ� ���Ͱ� �Ǵ� ����
// �ﰢ�Լ� ȣ�⿡�� 90% �̻��� �ð��� ��Դ´� --> ��� �ذ� ���!
//----------------------------------------------------------------------------------------//
D3DXVECTOR3 CMesh::DecodeNormal(short normal)
{
	D3DXVECTOR3 out;
	float lat, lng;

	lat = (float)(( normal >> 8 ) & 0xff);
	lng = (float)( normal & 0xff );
	lat *= (float)D3DX_PI/128;
	lng *= (float)D3DX_PI/128;

	out.x = cos(lat) * sin(lng);
	out.y = sin(lat) * sin(lng);
	out.z = cos(lng);

	return out;
}

void CMesh::Load(HANDLE &pFile)
{
	DWORD dwSize;
	DWORD dwFP,dwSFP;
	dwSFP = SetFilePointer(pFile, 0 ,NULL,FILE_CURRENT);
	//---------------------------------------------------------------------------------------------------------------
	// �ϴ� Mesh ���
	ReadFile(pFile,&m_MeshHeader,sizeof(MD3MESHHEADER),&dwSize, NULL);

	//---------------------------------------------------------------------------------------------------------------
	// ����ũ3���� ���Ǵ� ��ũ��Ʈ��� �� (�������� �����ϴ�, �׷��� ���̴��� �θ�)
	// ���� å���� �ؽ�ó �����̶�� ��
	// �� �κ��� �𸣰ڴ�. �Ѥ�; ��ư �ؽ�ó ���� * 68Byte ��
	m_pTexName = new MD3TEXNAME[m_MeshHeader.iTextureNum];
	ReadFile(pFile,m_pTexName, sizeof(MD3TEXNAME)*m_MeshHeader.iTextureNum, &dwSize, NULL);

	//---------------------------------------------------------------------------------------------------------------
	// �ε�����
	// ���� ����Ʈ�� ������ġ(�� �Ž��� ���ؽ� ������ġ)+�ﰢ����ŸƮ �������� ���� �� �д´�. 
	// �ȱ׷��� ������ ���������� ��찡 �����.
	dwFP = SetFilePointer(pFile, dwSFP + m_MeshHeader.iTriangleStart ,NULL,FILE_BEGIN);

	m_pTriangle = new MD3TRIANGLE[ m_MeshHeader.iTriangleNum ];
	ReadFile(pFile,m_pTriangle,sizeof(MD3TRIANGLE)*m_MeshHeader.iTriangleNum,&dwSize,NULL);

	//---------------------------------------------------------------------------------------------------------------
	// �ؽ�ó ��ǥ
	D3DXVECTOR2 *pTex = new D3DXVECTOR2[m_MeshHeader.iVertexNum];
		
	dwFP = SetFilePointer(pFile, dwSFP + m_MeshHeader.iTecVecStart, NULL, FILE_BEGIN);
	ReadFile(pFile, pTex, sizeof(D3DXVECTOR2) *m_MeshHeader.iVertexNum, &dwSize, NULL);

	//---------------------------------------------------------------------------------------------------------------
	//	���� ����� ���ؽ�
	//	�ϴ� ��ǥ�� ������ ����Ǿ� �ִ�
	//	FP�� �ٽ� �����. Ȥ�� �𸣴ϱ� ������ --;
	dwFP = SetFilePointer(pFile, dwSFP + m_MeshHeader.iVertexStart,NULL,FILE_BEGIN );
	MD3VERTEX_FILE *pFileVertex = new MD3VERTEX_FILE[ m_MeshHeader.iVertexNum * m_MeshHeader.iMeshFrameNum ];

	ReadFile(pFile, pFileVertex, sizeof(MD3VERTEX_FILE)*m_MeshHeader.iVertexNum * m_MeshHeader.iMeshFrameNum,&dwSize,NULL );

	m_pVertex = new D3DFVFVERTEX[ m_MeshHeader.iVertexNum * m_MeshHeader.iMeshFrameNum ];
	
	int i,j;
	for(i=0; i<m_MeshHeader.iMeshFrameNum * m_MeshHeader.iVertexNum; i++)
	{
		for(j=0; j<3; j++)
		{
			// ��ǥ���� short�� * 64 �� �ؼ� ���ڵ� ���ѳ���.
			// �׷��� �о���϶� 64�� ������.
			m_pVertex[i].vVector[j] = (float)pFileVertex[i].Vertex[j] / 64;
		}
		// ��ְ� ó�� �Ѵ� -_-
		m_pVertex[i].nNoraml = CMesh::DecodeNormal(pFileVertex[i].Normal);
		
		// ������ �о�� �ؽ�ó ��ǥ���� ����
		if( i < m_MeshHeader.iVertexNum )	// �ؽ�ó��ǥ�� ���ؽ� ������ŭ�� ���� �ȴ�.
			m_pVertex[i].Tex = pTex[i];
	}

	SAFE_DELETE_ARRAY(pTex);
	SAFE_DELETE_ARRAY(pFileVertex);
}

HRESULT CMesh::LoadTexture(char *szFileName)
{
	LPDIRECT3DTEXTURE9 pTexture = NULL;
	if(FAILED(D3DXCreateTextureFromFile(m_pd3dDevice,szFileName,&pTexture)) )
	{
		// Failed ~~~
		OutputDebugString("�ؽ�ó�ε� ����!!\n");
		return E_FAIL;
	}

	// ������ ���ؼ���...�̷��� -_-.
	m_listTexture.push_back(pTexture);
	return S_OK;
}

HRESULT CMesh::CreateVertexBuffer()
{
	if(FAILED( m_pd3dDevice->CreateVertexBuffer(m_MeshHeader.iVertexNum * sizeof(D3DFVFVERTEX),
									D3DUSAGE_DYNAMIC|D3DUSAGE_WRITEONLY,
									D3DFVF_VERTEX,D3DPOOL_DEFAULT, &m_pVB, NULL) ))
	{
		OutputDebugString("[����] CMesh::CreateVertexBuffer");
		return E_FAIL;
	}
	return S_OK;
}
HRESULT CMesh::CreateIndexBuffer()
{
	if(FAILED( m_pd3dDevice->CreateIndexBuffer( sizeof(WORD)* m_MeshHeader.iTriangleNum * sizeof(MD3TRIANGLE),
												D3DUSAGE_DYNAMIC|D3DUSAGE_WRITEONLY,D3DFMT_INDEX16,
												D3DPOOL_DEFAULT, &m_pIB,NULL)))
	{
		OutputDebugString("[����] CMesh::CreateIndexBuffer");
		return E_FAIL;
	}
	
	WORD *pIndices=NULL;
	if( FAILED( m_pIB->Lock(0,0, (void **)&pIndices, D3DLOCK_DISCARD) ))
	{
		return E_FAIL;
	}

	DWORD dwIndexCnt=0;
	for(int i=0; i<m_MeshHeader.iTriangleNum; i++)
	{
		for(int j=0; j<3; j++)
		{
			pIndices[dwIndexCnt++] = (WORD)m_pTriangle[i].Index[j];
		}
	}

	m_pIB->Unlock();

	return S_OK;
}

HRESULT CMesh::Render(UINT nCurFrame, UINT nNextFrame, float fInterPolation)
{
	D3DFVFVERTEX *pVertexBuffer=NULL;

	if( FAILED( m_pVB->Lock(0,0, (void **)&pVertexBuffer, D3DLOCK_DISCARD) ))
	{
		OutputDebugString("[Failed] CMesh::Render / Lcok\n");
		return E_FAIL;
	}

	int i;
	DWORD nCurVertex = nCurFrame * m_MeshHeader.iVertexNum;
	DWORD nNextVertex = nNextFrame * m_MeshHeader.iVertexNum;

	for(i=0; i<m_MeshHeader.iVertexNum; i++)
	{
		// ������ ���� ��ǥ : CurrVertex.vPosition +  fPol * (NextVertex.vPosition - CurrVertex.vPosition);
		pVertexBuffer[i].vVector = m_pVertex[nCurVertex+i].vVector + fInterPolation * 
									(m_pVertex[nNextVertex+i].vVector - m_pVertex[nCurVertex+i].vVector ) ;

		pVertexBuffer[i].nNoraml = m_pVertex[nCurFrame+i].nNoraml + fInterPolation * 
									(m_pVertex[nNextVertex+i].nNoraml - m_pVertex[nCurVertex+i].nNoraml );
		pVertexBuffer[i].Tex	 = m_pVertex[i].Tex;
	}
	
	if( FAILED( m_pVB->Unlock() ) )
	{
		OutputDebugString("[Failed] CMesh::Render / Unlock\n");
		return E_FAIL;
	}

	DWORD dwSize = m_listTexture.size();
	for(i=0; i<dwSize; ++i)
	{
		m_pd3dDevice->SetTexture(i,m_listTexture[i]);
	}
	
	m_pd3dDevice->SetIndices(m_pIB);
	m_pd3dDevice->SetStreamSource(0,m_pVB,0,sizeof(D3DFVFVERTEX) );
	m_pd3dDevice->SetFVF(D3DFVF_VERTEX);	
	m_pd3dDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST,0,0, m_MeshHeader.iVertexNum,0,m_MeshHeader.iTriangleNum);
	
	return S_OK;
}
