#include "stdafx.h"

CUtility::sFrameSkip	CUtility::g_FrameSkip;
DWORD					CUtility::g_dwCurrentTime;

CUtility& CUtility::GetInstance()
{
	static CUtility instance;
	return instance;
}

CUtility::sFrameSkip::sFrameSkip()
{
	m_fTime = 0;
	m_fRate = FRAME_VELOCITY;
	m_nFps = 0;
}

BOOL CUtility::sFrameSkip::Skip()
{
	if ( m_fTime==0 )	ResetFrame();

	while( Wait() )	Sleep(1);
	m_fTime -= 1000.0f/m_fRate;

	if ( m_fTime<0 )	
		return true;
	else
		return false;
}

int CUtility::sFrameSkip::Wait(void)
{
	GetTime();

	if ( m_fTime<0 )	
		return 1;
	else
		return 0;
}

void CUtility::sFrameSkip::GetTime(void)
{
	DWORD	dwTime;

	dwTime = timeGetTime();

	m_fTime += dwTime - m_dwTimeSaving;
	m_dwTimeSaving = dwTime;
}

void CUtility::sFrameSkip::ResetFrame(void)
{
	m_dwTimeSaving = timeGetTime();
}

void CUtility::ResetFrame()
{
	g_FrameSkip.ResetFrame();
	g_FrameSkip.m_fTime = 0;
}

BOOL CUtility::FrameSkip()
{
	return g_FrameSkip.Skip();
}

void CUtility::SetTimer(DWORD dwTime)
{
	g_dwCurrentTime = dwTime;
}

float CUtility::AngleBetweenV( D3DXVECTOR3 &P1, D3DXVECTOR3 &P2 )
{
	float	dot = D3DXVec3Dot( &P1, &P2 );
	float   mag = D3DXVec3Length( &P1 ) * D3DXVec3Length( &P2 );
	double  ang = acos( dot / mag );

	if ( _isnan( ang ) ) return 0.0f;

	return (float)ang;
}

float CUtility::FrameSecDist(float fSpeed, DWORD dwElapsed )
{
	float dt = (float)dwElapsed/1000; // 초 단위로 만듬
	return (fSpeed*dt ) + (dt*dt)*0.5f;	// dt^2*0.5가 이해 안됨..왜 dt의 평균을 구할까
}

char* CUtility::GetAssetPath( const string& fileName )
{
	char szBuffer[4096] = { 0, };
	sprintf( szBuffer, "%s\\%s", m_rootPath.c_str(), fileName.c_str() );

	int size = strlen( szBuffer ) + 1;
	char* pPice = new char[ size ];
	ZeroMemory( pPice, size );
	strcpy( pPice, szBuffer );

	m_store.push_back( pPice );

	return pPice;
}