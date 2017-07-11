//------------------------------------------------------------------------------
// File. XFile.cpp
//------------------------------------------------------------------------------

#include "StdAfx.h"

CXFile::CXFile():
m_vPos(0,0,0),
m_pD3DDevice(NULL),
m_pMesh(NULL),
m_pTex(NULL),
m_pMaterials(NULL),
m_dwNumMaterials(0),
m_pUserTexture(NULL),
m_pSetTexture(NULL),
m_bCartoon(0),
m_pInverseView(NULL)
{
	D3DXMatrixIdentity(&m_matScale);
	D3DXMatrixIdentity(&m_matWorld);
	D3DXQuaternionIdentity(&m_quateRotate);
}
CXFile::~CXFile()
{
}

HRESULT CXFile::Create(char *strFileName,LPDIRECT3DDEVICE9 Device, bool bCartoon)
{
	m_bCartoon = bCartoon;
	m_pD3DDevice = Device;
	LPD3DXBUFFER pAdjacencyBuffer = NULL;
	LPD3DXBUFFER pMtrlBuffer = NULL;

	LPD3DXMESH pMesh=NULL;
		
	// �޽� �ε�
	if( FAILED(D3DXLoadMeshFromX( strFileName, D3DXMESH_SYSTEMMEM, m_pD3DDevice,
		&pAdjacencyBuffer, &pMtrlBuffer, NULL,&m_dwNumMaterials, &pMesh) ) )
	{
		return E_FAIL;
	}

	pMesh->CloneMeshFVF( D3DXMESH_MANAGED|D3DXMESH_WRITEONLY ,
		D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_TEX1 , m_pD3DDevice , &m_pMesh );
	
	// ����
	if( pMtrlBuffer !=0 && m_dwNumMaterials != 0 )
	{
		D3DXMATERIAL* d3dxMtrls = (D3DXMATERIAL*)pMtrlBuffer->GetBufferPointer();
		m_pMaterials = new D3DMATERIAL9[m_dwNumMaterials];
		if( NULL == m_pMaterials ){	return E_FAIL;	}

		m_pTex = new LPDIRECT3DTEXTURE9[m_dwNumMaterials];
		if( NULL == m_pTex ){ return E_FAIL; }
		
		bool bTexLoad = false;
		DWORD i;
		for(i=0; i<m_dwNumMaterials; i++ )
		{
			m_pMaterials[i]		= d3dxMtrls[i].MatD3D;
			m_pTex[i]			= NULL;
			
			// Create a texture
			if( d3dxMtrls[i].pTextureFilename )
			{
				if( SUCCEEDED( D3DXCreateTextureFromFile( m_pD3DDevice, d3dxMtrls[i].pTextureFilename,&m_pTex[i] ) ) )
				{
					bTexLoad = true;
				}
			}
		}//end for

		// �ؽ�ó ����
		if(!bTexLoad)	// �ؽ�ó�� ���� ���ߴٸ�
		{
			SAFE_DELETE_ARRAY(m_pTex);	// ���� �ع�����
		}
	}

	return S_OK;
}

HRESULT	CXFile::CreateTexture(char *strFileName)
{
	if( FAILED( D3DXCreateTextureFromFileEx( m_pD3DDevice, strFileName, 
		D3DX_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT, 0, D3DFMT_UNKNOWN,
		D3DPOOL_MANAGED, D3DX_FILTER_TRIANGLE|D3DX_FILTER_MIRROR,
		D3DX_FILTER_TRIANGLE|D3DX_FILTER_MIRROR, 0, NULL, NULL, &m_pUserTexture ) ) )
	{
		return E_FAIL;
	}
	return S_OK;
}
void CXFile::SetWorldTransform()
{
	D3DXMATRIX matTrans;
	D3DXMatrixTranslation( &matTrans,m_vPos.x,m_vPos.y,m_vPos.z);

	D3DXMATRIX matRotate;
	D3DXMatrixRotationQuaternion(&matRotate,&m_quateRotate);

	if( m_pInverseView )
		m_matWorld = m_matScale * matRotate * matTrans * (*m_pInverseView);
	else
		m_matWorld = m_matScale * matRotate * matTrans;

	m_pD3DDevice->SetTransform( D3DTS_WORLD, &m_matWorld);
}

bool CXFile::DrawSubset()
{
	this->SetWorldTransform();
	
	DWORD i;
	for(i=0; i<m_dwNumMaterials; ++i)
	{
		// ����
		m_pMaterials[i].Ambient = m_pMaterials[i].Diffuse;
		m_pD3DDevice->SetMaterial(&m_pMaterials[i]);

		// ���� �⺻ �ؽ�ó�� �ִٸ� ����
		// ���ٸ�, ���������� �ؽ�ó�� �ִ��� Ȯ��
		// ���ٸ� �ؽ�ó�� �η� �����Ѵ�.
		if( m_pTex ) m_pD3DDevice->SetTexture( 0, m_pTex[i] );	// �ؽ�ó�� �������̶�� �տ��� ���ؾ� �Ǵ°� �ƴѰ�
		else if( m_pSetTexture ) m_pD3DDevice->SetTexture(0, m_pSetTexture );
		else if( m_pUserTexture ) m_pD3DDevice->SetTexture( 0, m_pUserTexture );
		else m_pD3DDevice->SetTexture(0,NULL);

		m_pMesh->DrawSubset( i );
	}
	return true;
}

bool CXFile::Render(CRender * pRender/*=NULL*/)
{
	this->SetWorldTransform();
	
	// Render State
	m_pD3DDevice->SetRenderState( D3DRS_FOGENABLE,    TRUE );
	m_pD3DDevice->SetRenderState( D3DRS_CULLMODE,	  D3DCULL_CCW );
	
	// ���� ������ ī���� �ƴ϶��  -> FVF
	if( !m_bCartoon || NULL == pRender || pRender->GetType() != RT_Cartoon )
	{
		m_pD3DDevice->SetRenderState( D3DRS_LIGHTING,	  TRUE );

		DWORD i;
		for(i=0; i<m_dwNumMaterials; ++i)
		{
			// ����
			m_pMaterials[i].Ambient = m_pMaterials[i].Diffuse;
			m_pD3DDevice->SetMaterial(&m_pMaterials[i]);
			
			// ���� �⺻ �ؽ�ó�� �ִٸ� ����
			// ���ٸ�, ���������� �ؽ�ó�� �ִ��� Ȯ��
			// ���ٸ� �ؽ�ó�� �η� �����Ѵ�.
			if( m_pTex ) m_pD3DDevice->SetTexture( 0, m_pTex[i] );	// �ؽ�ó�� �������̶�� �տ��� ���ؾ� �Ǵ°� �ƴѰ�
			else if( m_pUserTexture ) m_pD3DDevice->SetTexture( 0, m_pUserTexture );
			else m_pD3DDevice->SetTexture(0,NULL);
			
			m_pMesh->DrawSubset( i );
		}
	}
	else // �ƴ϶��, �� ���̴�
	{
		// ����ϱ� ���ϰ� �����͸� ����
		CRender_Cartoon * pCartoon = (CRender_Cartoon *)pRender;
				
		D3DXVECTOR3 vLightDir(-1.f,-1.f,1.f);
		D3DXVec3Normalize( &vLightDir, &vLightDir);
		D3DXVECTOR4 vC0(0.f, 0.5f, 1.0f, 2.0f);
		
		pCartoon->BeginScene(&m_matWorld,vLightDir,vC0);
		
		// ���� ������Ƽ��� �׷���� ������ ��..
		DWORD i;
		for(i=0; i<m_dwNumMaterials; ++i)
		{
			D3DXVECTOR4 vColor(m_pMaterials[i].Diffuse.r,m_pMaterials[i].Diffuse.g,m_pMaterials[i].Diffuse.b,0);
			pCartoon->SetSurfaceShader(vColor);
			m_pMesh->DrawSubset( i );	
		}
		
		pCartoon->SetSiluouetteShader();
		for(i=0; i<m_dwNumMaterials; ++i)
		{	m_pMesh->DrawSubset( i );	}
		
		pCartoon->EndScene();
	}
	
	return true;
}
