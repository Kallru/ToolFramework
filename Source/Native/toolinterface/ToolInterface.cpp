// ToolInterface.cpp : DLL 응용 프로그램을 위해 내보낸 함수를 정의합니다.
//

#include "ToolPCH.h"
#include <boost/algorithm/string.hpp>
#include "../FrameWork/StdAfx.h"

ToolInterface::ToolInterface( ::HINSTANCE applicationInstance, ::HWND hWndParent )
: m_pClient( NULL )
, m_applicationInstance( applicationInstance )
, m_hWndParent( hWndParent )
, m_windowWidth( 1 )
, m_windowHeight( 1 )
{

}

ToolInterface::~ToolInterface()
{
}

bool ToolInterface::Create( ::HWND windowHandle, const std::string& dataPath )
{
	m_pClient = new CGameMain( windowHandle, 0, 0, dataPath, D3DCREATE_HARDWARE_VERTEXPROCESSING );
	return true;
}

bool ToolInterface::Run()
{
	return m_pClient->Run();
}

bool ToolInterface::Stop()
{
	return m_pClient->Stop();
}

LRESULT ToolInterface::MessageHandler(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
	return m_pClient->MessageHandler( hWnd, Message, wParam, lParam );
}

void ToolInterface::ResetBuffer()
{
	m_outputBuffer.Clear();
}

int ToolInterface::GetBufferLenght()
{
	return m_outputBuffer.GetSize();
}

void* ToolInterface::GetRawPointer()
{ 
	return m_outputBuffer.GetStream();
}

bool ToolInterface::CommandHandler( const std::string& command, BinaryReader& input )
{
	std::string lowerCommand( command );
	boost::algorithm::to_lower( lowerCommand );

	return m_commandMap[ lowerCommand ]( this, input );
}

bool ToolInterface::CommandHandlerOutput( const std::string& command, BinaryReader& input )
{
	std::string lowerCommand( command );
	boost::algorithm::to_lower( lowerCommand );

	return m_commandOutputMap[ lowerCommand ]( this, input, m_outputBuffer );
}
