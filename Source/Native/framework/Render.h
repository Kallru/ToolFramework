//----------------------------------------------------------------------------------
// File. Render.h
// Note. 어차피 쓰는건 카툰 한개라서 랜더 클래스가 무의미하긴 함..ㅋ
//----------------------------------------------------------------------------------

#ifndef _RENDER_
#define _RENDER_

enum RenderType { RT_Unkown=0, RT_Cartoon };

class CRender
{
public:
	CRender();
	virtual ~CRender();

	// 부모에서 해줄건 없다~
	virtual bool FrameMove() = 0;

	RenderType GetType() { return m_nType; }

protected:
	RenderType m_nType;
};
#endif