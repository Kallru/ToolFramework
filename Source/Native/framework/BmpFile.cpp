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

	// ��� �б�
	ReadFile(hRfpt,&m_tagBMPFile,sizeof(BITMAPFILEHEADER),&dwRSize,NULL);
	ReadFile(hRfpt,&m_tagBMPInfo,sizeof(BITMAPINFOHEADER),&dwRSize,NULL);

	// Bmp File Check
	if(m_tagBMPFile.bfType != 0x4d42){ MessageBox(NULL, "BMP ���� ������ �ƴմϴ�.", "Error",MB_OK);return E_FAIL; }
	// 24bit�� �����Ѵ�. �ٸ��� ���� �ٸ���� -_-
	if(m_tagBMPInfo.biBitCount != 0x18){ MessageBox(NULL, "24bit BMP ���ĸ� �����մϴ�.","Error",MB_OK);return E_FAIL; }

	// ������
	m_Image.dwHeight= m_tagBMPInfo.biHeight;
	m_Image.dwWidth = m_tagBMPInfo.biWidth;

	// �̹��� ����� ���Ѵ�.
	DWORD dwBmpSize = 0;
	if(0 == m_tagBMPInfo.biSizeImage)	// ���� ������ ����� ���ٸ�
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
	WORD wBPP = m_tagBMPInfo.biBitCount/8;	// ���ص���, ������ 24/8 �ε�
	
	DWORD dwY = m_Image.dwHeight - y - 1;	// �������� ������ �ϴϱ�..
	DWORD dwX = x*wBPP;		// 4����Ʈ ���� ������ x

	// ��ġ�� ����ô�~
	DWORD dwPitch;
	dwPitch = m_Image.dwWidth * wBPP;
	dwPitch = this->Sort4b(dwPitch);

	return m_Image.Data[dwX+dwY*dwPitch];
}