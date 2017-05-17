#include <Windows.h>
#include "LocateWindowHandles.h"
#include <stdint.h>

class WinCap {

public:
	//Device Contexts
	HDC hdcSource;
	HDC hdcMemDC = NULL;

	//For saving bitmap
	HBITMAP hbmSource = NULL;
	BITMAP  bmpDest;

	//Window Handles
	HWND source;
	HWND dest;

	//Bitmap Dimensions
	RECT sourceRect;
	int sourceWidth;
	int sourceHeight;
	int pitch;

	//Bitmap
	uint8_t *bitmap;
	DWORD dwBmpSize;
	HANDLE hDIB;
	BITMAPINFOHEADER bi;
	int bytesPerPixel;


	WinCap(HWND _source);
	~WinCap() {
		GlobalUnlock(hDIB);
		GlobalFree(hDIB);

		DeleteObject(hbmSource);
		DeleteObject(hdcMemDC);
		ReleaseDC(NULL, hdcSource);
	}

	//Copy the contents of the source window into the (dest) bitmap
	void updateBitmap();

	//Write the (dest) bitmap to a file with the name FILENAME
	void saveBitmap(const char *FILENAME);

	//Update the dimensions variables
	void onResize();

	//Flip the image in pointed by bitmap
	void flipBitmap();

	uint32_t getPixel(int x, int y) {
		if (y != sourceHeight && x != sourceWidth)
		return *((uint32_t*)bitmap + (y * sourceWidth) + x);
	}

	void setPixel(int x, int y, uint32_t Color) {
		if(y!=sourceHeight && x!=sourceWidth)
		*((uint32_t*)bitmap + (y * sourceWidth) + x) = Color;
	}
};