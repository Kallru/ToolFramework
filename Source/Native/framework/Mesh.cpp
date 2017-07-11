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
// 기하학적으로 생각해보자. 
// 입체구에서 한점(위도/경도)을 표시하면 그것이 원점에서 그점을 가리켜서 벡터가 되는 원리
// 삼각함수 호출에서 90% 이상의 시간을 까먹는다 --> 요거 해결 요망!
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
	// 일단 Mesh 헤더
	ReadFile(pFile,&m_MeshHeader,sizeof(MD3MESHHEADER),&dwSize, NULL);

	//---------------------------------------------------------------------------------------------------------------
	// 퀘이크3에서 사용되는 스크립트라고 함 (조명같은거 조정하는, 그래서 쉐이더라 부름)
	// 흐음 책에선 텍스처 네임이라고 함
	// 이 부분은 모르겠다. ㅡㅡ; 여튼 텍스처 개수 * 68Byte 임
	m_pTexName = new MD3TEXNAME[m_MeshHeader.iTextureNum];
	ReadFile(pFile,m_pTexName, sizeof(MD3TEXNAME)*m_MeshHeader.iTextureNum, &dwSize, NULL);

	//---------------------------------------------------------------------------------------------------------------
	// 인덱스값
	// 파일 포인트를 시작위치(전 매쉬의 버텍스 다음위치)+삼각형스타트 지점으로 돌린 후 읽는다. 
	// 안그러면 위에랑 섞여버리는 경우가 생긴다.
	dwFP = SetFilePointer(pFile, dwSFP + m_MeshHeader.iTriangleStart ,NULL,FILE_BEGIN);

	m_pTriangle = new MD3TRIANGLE[ m_MeshHeader.iTriangleNum ];
	ReadFile(pFile,m_pTriangle,sizeof(MD3TRIANGLE)*m_MeshHeader.iTriangleNum,&dwSize,NULL);

	//---------------------------------------------------------------------------------------------------------------
	// 텍스처 좌표
	D3DXVECTOR2 *pTex = new D3DXVECTOR2[m_MeshHeader.iVertexNum];
		
	dwFP = SetFilePointer(pFile, dwSFP + m_MeshHeader.iTecVecStart, NULL, FILE_BEGIN);
	ReadFile(pFile, pTex, sizeof(D3DXVECTOR2) *m_MeshHeader.iVertexNum, &dwSize, NULL);

	//---------------------------------------------------------------------------------------------------------------
	//	드디어 대망에 뽀텍스
	//	일단 좌표와 법선이 압축되어 있다
	//	FP를 다시 맞춘다. 혹시 모르니까 맞추자 --;
	dwFP = SetFilePointer(pFile, dwSFP + m_MeshHeader.iVertexStart,NULL,FILE_BEGIN );
	MD3VERTEX_FILE *pFileVertex = new MD3VERTEX_FILE[ m_MeshHeader.iVertexNum * m_MeshHeader.iMeshFrameNum ];

	ReadFile(pFile, pFileVertex, sizeof(MD3VERTEX_FILE)*m_MeshHeader.iVertexNum * m_MeshHeader.iMeshFrameNum,&dwSize,NULL );

	m_pVertex = new D3DFVFVERTEX[ m_MeshHeader.iVertexNum * m_MeshHeader.iMeshFrameNum ];
	
	int i,j;
	for(i=0; i<m_MeshHeader.iMeshFrameNum * m_MeshHeader.iVertexNum; i++)
	{
		for(j=0; j<3; j++)
		{
			// 좌표값이 short형 * 64 를 해서 인코딩 시켜놨다.
			// 그래서 읽어들일때 64를 나눈다.
			m_pVertex[i].vVector[j] = (float)pFileVertex[i].Vertex[j] / 64;
		}
		// 노멀값 처리 한당 -_-
		m_pVertex[i].nNoraml = CMesh::DecodeNormal(pFileVertex[i].Normal);
		
		// 위에서 읽어둔 텍스처 좌표값도 셋팅
		if( i < m_MeshHeader.iVertexNum )	// 텍스처좌표는 버텍스 개수만큼만 돌면 된다.
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
		OutputDebugString("텍스처로드 실패!!\n");
		return E_FAIL;
	}

	// 나중을 위해서라도...이렇게 -_-.
	m_listTexture.push_back(pTexture);
	return S_OK;
}

HRESULT CMesh::CreateVertexBuffer()
{
	if(FAILED( m_pd3dDevice->CreateVertexBuffer(m_MeshHeader.iVertexNum * sizeof(D3DFVFVERTEX),
									D3DUSAGE_DYNAMIC|D3DUSAGE_WRITEONLY,
									D3DFVF_VERTEX,D3DPOOL_DEFAULT, &m_pVB, NULL) ))
	{
		OutputDebugString("[실패] CMesh::CreateVertexBuffer");
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
		OutputDebugString("[실패] CMesh::CreateIndexBuffer");
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
		// 보간된 최종 좌표 : CurrVertex.vPosition +  fPol * (NextVertex.vPosition - CurrVertex.vPosition);
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
