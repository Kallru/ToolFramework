//---------------------------------------------------------------------------------------
//	File. Timer.h
//---------------------------------------------------------------------------------------

#ifndef _TIMER_
#define _TIMER_

class CTimer
{
public:
	CTimer() 
	{
		m_Timers.push_back(this);
		Stop();
	}
	virtual ~CTimer() 
	{
		std::vector<CTimer*>::iterator itor;
		for ( itor = m_Timers.begin(); itor != m_Timers.end(); itor++) 
		{
			if ((*itor) == this) 
			{
				m_Timers.erase(itor);
				break;
			}
		}
	}

public:
	void Start() { m_bIsRunning = true;  }
	void Pause() { m_bIsRunning = false; }
	void Stop()  { Pause(); m_fTime = 0; }
	void Begin() { Stop(); Start(); }

	float GetTime() { return(m_fTime); }
	float GetElaps() { return m_fElapsedTime; }
	
	bool IsRunning() { return(m_bIsRunning); }
	
	void Update(float fElapsedTime) 
	{
		if (m_bIsRunning) 
		{
			m_fTime += fElapsedTime;
			m_fElapsedTime = fElapsedTime;
		}
	}
	
	static std::vector<CTimer *> m_Timers;
	static void UpdateAll(float fElapsedTime) 
	{
		for (std::vector<CTimer*>::iterator i = m_Timers.begin(); i != m_Timers.end(); i++) 
		{	(*i)->Update(fElapsedTime);  }
	}
	
protected:
	bool m_bIsRunning;
	float m_fTime;

	float m_fElapsedTime;

};

#endif