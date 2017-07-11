//------------------------------------------------------------------------------
// File. WinMain.h
//------------------------------------------------------------------------------

#ifndef _WINMAIN_
#define _WINMAIN_

INT WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPreInst, LPSTR lpCmdLine, INT nShowCmd);
LRESULT WINAPI EntryMessageHandler(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam);

#endif