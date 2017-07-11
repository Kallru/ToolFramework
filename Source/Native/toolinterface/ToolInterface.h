#pragma once

class CD3DApp;
class ToolInterface
{
private:
	typedef std::function<bool ( ToolInterface*, BinaryReader& input )> CommandType;
	typedef std::function<bool ( ToolInterface*, BinaryReader& input, BinaryWriter& output )> CommandOutputType;

	typedef std::map<std::string, CommandType> CommandMap;
	typedef std::map<std::string, CommandOutputType> CommandOutputMap;

public:
	ToolInterface( ::HINSTANCE applicationInstance, ::HWND hWndParent );
	~ToolInterface();

	bool Create( ::HWND windowHandle, const std::string& dataPath );

	bool Run();
	bool Stop();

	LRESULT MessageHandler(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam);

	bool CommandHandler( const std::string& command, BinaryReader& input );
	bool CommandHandlerOutput( const std::string& command, BinaryReader& input );

	void ResetBuffer();
	int GetBufferLenght();
	void* GetRawPointer();

private:
	CD3DApp* m_pClient;

	::HINSTANCE m_applicationInstance;
	::HWND m_hWndParent;

	int m_windowWidth;
	int m_windowHeight;

	BinaryWriter m_outputBuffer;
	
	CommandMap m_commandMap;
	CommandOutputMap m_commandOutputMap;
};