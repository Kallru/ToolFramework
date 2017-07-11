//------------------------------------------------------------------------------
// File. D3DApplication.cpp
//------------------------------------------------------------------------------

#include "StdAfx.h"
#include "D3DApp.h"

CD3DApp::CD3DApp()
{
}

CD3DApplication::~CD3DApplication()
{
}

HRESULT CD3DApplication::Create()
{
	return S_OK;
}
bool CD3DApplication::Run()
{
	return true;
}

LRESULT WINAPI CD3DApplication::MessageHandler(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
	return DefWindowProc(hWnd, Message, wParam, lParam);
}