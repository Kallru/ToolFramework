#pragma once

#define DLL_EXPORT __declspec(dllexport)

extern "C"
{
	// 툴 인터페이스 초기화
	DLL_EXPORT void* InitalizeWindow( ::HINSTANCE applicationInstance, ::HWND hWndParent, int screenWidth, int screenHeight, const char* pAssetPath );

	// 툴 인터페이스 해제
	DLL_EXPORT void Start();
	DLL_EXPORT void Stop();

	DLL_EXPORT bool Send( const char* pCommand, void* pData, unsigned int dataSize );
	DLL_EXPORT bool SendOutput( const char* pCommand, void* pData, unsigned int dataSize, void*& pOutputData, unsigned int& outputDataSize );
	
}