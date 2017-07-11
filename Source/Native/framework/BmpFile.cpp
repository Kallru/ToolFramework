// File. BmpFile.cpp

#include "StdAfx.h"
#include "BmpFile.h"

CBmpFile::CBmpFile()
{
}

CBmpFile::~CBmpFile()
{
}

HRESULT CBmpFile::LoadFile(LPSTR lpFileName)
{
	HANDLE hRfpt;
	DWORD dwRSize;
	
	hRfpt = CreateFile(lpFileName,GENERIC_READ,0,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
	if(INVALID_HANDLE_VALUE==hRfpt)
	{
		return E_FAIL;
	}

	// 헤더 읽기
	ReadFile(hRfpt,&m_tagBMPFile,sizeof(BITMAPFILEHEADER),&dwRSize,NULL);
	ReadFile(hRfpt,&m_tagBMPInfo,sizeof(BITMAPINFOHEADER),&dwRSize,NULL);

	// Bmp File Check
	if(m_tagBMPFile.bfType != 0x4d42){ MessageBox(NULL, "BMP 파일 형식이 아닙니다.", "Error",MB_OK);return E_FAIL; }
	// 24bit만 지원한다. 다른거 언제 다만들어 -_-
	if(m_tagBMPInfo.biBitCount != 0x18){ MessageBox(NULL, "24bit BMP 형식만 지원합니다.","Error",MB_OK);return E_FAIL; }

	// 사이즈
	m_Image.dwHeight= m_tagBMPInfo.biHeight;
	m_Image.dwWidth = m_tagBMPInfo.biWidth;

	// 이미지 사이즈를 구한다.
	DWORD dwBmpSize = 0;
	if(0 == m_tagBMPInfo.biSizeImage)	// 만약 인포에 사이즈가 없다면
	{
		dwBmpSize = this->Sort4b(m_tagBMPInfo.biWidth*3)*m_tagBMPInfo.biHeight;
	}
	else
		dwBmpSize = m_tagBMPInfo.biSizeImage;

	m_Image.dwSize  = dwBmpSize;
	m_Image.Data = new BYTE[ m_Image.dwSize ];
	ZeroMemory(m_Image.Data,m_Image.dwSize);

	ReadFile(hRfpt,&m_Image.Data[0],m_Image.dwSize,&dwRSize,NULL);
	CloseHandle(hRfpt);

	return S_OK;
}

DWORD CBmpFile::Sort4b(DWORD dwN)
{
	WORD wRemainder = dwN%4;
	if(wRemainder)	return dwN + 4 - wRemainder;
	else return dwN;
}

BYTE CBmpFile::GetData(int x,int y)
{
	WORD wBPP = m_tagBMPInfo.biBitCount/8;	// 구해두자, 어차피 24/8 인데
	
	DWORD dwY = m_Image.dwHeight - y - 1;	// 역순으로 만들어야 하니까..
	DWORD dwX = x*wBPP;		// 4바이트 정렬 기준의 x

	// 피치를 만듭시다~
	DWORD dwPitch;
	dwPitch = m_Image.dwWidth * wBPP;
	dwPitch = this->Sort4b(dwPitch);

	return m_Image.Data[dwX+dwY*dwPitch];
}