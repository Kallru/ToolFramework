
#include "ToolPCH.h"
#include "DllExport.h"
#include <memory>

extern "C"
{	
	static std::shared_ptr<ToolInterface> g_pToolInterface;
	LRESULT WINAPI EntryMessageHandler(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam)
	{
		if( g_pToolInterface.get() != NULL )
		{
			return g_pToolInterface->MessageHandler( hWnd, Message, wParam, lParam );
		}
		
		return DefWindowProc(hWnd, Message, wParam, lParam);
	}

	void* InitalizeWindow( ::HINSTANCE applicationInstance, ::HWND hWndParent, int screenWidth, int screenHeight, const char* pAssetPath )
	{
		WNDCLASS wndClass = 
		{ 
			CS_DBLCLKS, 
			EntryMessageHandler, 
			0, 0, applicationInstance, NULL, LoadCursor( NULL, IDC_ARROW ),(HBRUSH)GetStockObject( BLACK_BRUSH ),
			NULL, "ToolFramwork"
		};

		RegisterClass( &wndClass );

		// 윈도우 생성  
		HWND windowHandle = ::CreateWindow( 
			"ToolFramwork",
			NULL,
			WS_CHILD,
			CW_USEDEFAULT,
			0,
			1,
			1,
			hWndParent,
			NULL,
			applicationInstance,
			NULL );

		if( windowHandle == NULL )
		{
			// Error
			return NULL;
		}

		::UpdateWindow( windowHandle );

		// 툴 인터페이스 초기화
		g_pToolInterface.reset( new ToolInterface( applicationInstance, hWndParent ) );
		if( !g_pToolInterface->Create( windowHandle, std::string( pAssetPath ) ) )
		{
			g_pToolInterface.reset();
			return NULL;
		}

		return reinterpret_cast<void*>( windowHandle );
	}

	void Start()
	{
		if( g_pToolInterface.get() != NULL )
		{
			g_pToolInterface->Run();
			g_pToolInterface.reset();
		}
	}	

	void Stop()
	{
		if( g_pToolInterface.get() != NULL )
		{
			g_pToolInterface->Stop();
			g_pToolInterface.reset();
		}
	}

	bool Send( const char* pCommond, void* pData, unsigned int dataSize )
	{
		if( g_pToolInterface.get() != NULL )
		{
			BinaryReader input( pData, dataSize );
			return g_pToolInterface->CommandHandler( std::string( pCommond ), input );
		}

		return false;
	}

	bool SendOutput( const char* pCommond, void* pData, unsigned int dataSize, void*& pOutputData, unsigned int& outputDataSize )
	{
		if( g_pToolInterface.get() != NULL )
		{
			BinaryReader input( pData, dataSize );

			g_pToolInterface->ResetBuffer();
			bool bResult = g_pToolInterface->CommandHandlerOutput( std::string( pCommond ), input );
			if( bResult )
			{
				outputDataSize = g_pToolInterface->GetBufferLenght();
				pOutputData = g_pToolInterface->GetRawPointer();
				return true;
			}
		}

		return false;
	}

	int WINAPI WinMain( __in HINSTANCE /*hInstance*/, __in_opt HINSTANCE /*hPrevInstance*/, __in LPSTR /*lpCmdLine*/, __in int /*nShowCmd*/ )
	{
		BinaryWriter bw;
		bw.Write<int>( 1 );
		bw.Write<unsigned int>( 2 );
		bw.Write<char>( 3 );
		bw.Write<float>( 4.5f );
		bw.Write<std::string>( std::string( "Hello World" ) );
		bw.Write<double>( 8.5 );

		BinaryReader br( bw.GetStream(), bw.GetSize() );
		int a = br.Read<int>();
		unsigned int b = br.Read<unsigned int>();
		char c = br.Read<char>();
		float d = br.Read<float>();
		std::string e = br.Read<std::string>();
		double f = br.Read<double>();
		return 0;
	}
}