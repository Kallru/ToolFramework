// FrameSkip.h

#ifndef	__FRAMESKIP_H
#define	__FRAMESKIP_H

#define	FRAME_VELOCITY	40.0f

class CUtility
{
public:
	struct sFrameSkip
	{
		sFrameSkip();
		
		BOOL	Skip (void);
		int		Wait (void);
		void	GetTime (void);
		void	ResetFrame (void);
		
		float	m_fTime;
		int		m_nFps;
		float	m_fRate;
		DWORD	m_dwTimeSaving;
	};
	
	static void		ResetFrame();
	static BOOL		FrameSkip();
	static void		SetTimer(DWORD dwTime);

	static float	AngleBetweenV( D3DXVECTOR3 &P1, D3DXVECTOR3 &P2 );
	static float	FrameSecDist(float speed,DWORD time);

	char*	GetAssetPath( const string& fileName );

	static CUtility& GetInstance();

public:
	string m_rootPath;

private:
	static sFrameSkip  g_FrameSkip;
	static DWORD	   g_dwCurrentTime;

	vector<char*>  m_store;
};

#define g_Utility CUtility::GetInstance()

#endif