// File. BmpFile.h
// �� �ʿ��Ѱ�~

#ifndef _BMPFILE_
#define _BMPFILE_

typedef struct IMAGEDATA
{
	DWORD dwSize;
	DWORD dwHeight;
	DWORD dwWidth;
	BYTE *Data;

	IMAGEDATA::IMAGEDATA():Data(NULL),dwSize(0),dwHeight(0),dwWidth(0){}
	IMAGEDATA::~IMAGEDATA()	// Ȥ��, ������ ��������� ������..
	{
		if( dwSize > 1 ){	delete []Data;	}
		else{	delete Data;	}
		Data = NULL;
	}
}IMAGEDATA, *LPIMAGEDATA;

class CBmpFile
{
public:
	CBmpFile();
	~CBmpFile();
	
	HRESULT LoadFile(LPSTR lpFileName);

	BYTE GetData(int x,int y);

	DWORD GetWidth() { return m_Image.dwWidth; }
	DWORD GetHeight() { return m_Image.dwHeight; }

	DWORD Sort4b(DWORD dwN);

private:

	// Bmp Header
	BITMAPFILEHEADER m_tagBMPFile;
	BITMAPINFOHEADER m_tagBMPInfo;

	IMAGEDATA m_Image;
};

#endif
