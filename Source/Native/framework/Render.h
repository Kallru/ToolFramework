//----------------------------------------------------------------------------------
// File. Render.h
// Note. ������ ���°� ī�� �Ѱ��� ���� Ŭ������ ���ǹ��ϱ� ��..��
//----------------------------------------------------------------------------------

#ifndef _RENDER_
#define _RENDER_

enum RenderType { RT_Unkown=0, RT_Cartoon };

class CRender
{
public:
	CRender();
	virtual ~CRender();

	// �θ𿡼� ���ٰ� ����~
	virtual bool FrameMove() = 0;

	RenderType GetType() { return m_nType; }

protected:
	RenderType m_nType;
};
#endif