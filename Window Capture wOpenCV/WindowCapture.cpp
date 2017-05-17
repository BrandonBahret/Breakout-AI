#include "WindowCapture.h"

WinCap::WinCap(HWND _source) {
	source = _source;

	// Retrieve the handle to a display device context for the client 
	// area of the destination.
	hdcSource = GetDC(source);

	// Create a compatible DC which is used in a BitBlt from the destination DC
	hdcMemDC = CreateCompatibleDC(hdcSource);
	if (!hdcMemDC) {
		MessageBoxA(source, "CreateCompatibleDC has failed", "Failed", MB_OK);
		return;
	}

	onResize();

	// Create a compatible bitmap from the destination DC
	hbmSource = CreateCompatibleBitmap(hdcSource, sourceWidth, sourceHeight);
	if (!hbmSource)
	{
		MessageBoxA(source, "CreateCompatibleBitmap Failed", "Failed", MB_OK);
		return;
	}

	// Select the compatible bitmap into the compatible memory DC.
	SelectObject(hdcMemDC, hbmSource);

	// Get the BITMAP from the HBITMAP
	GetObject(hbmSource, sizeof(BITMAP), &bmpDest);

	//Fill out the bitmap info header
	bi.biSize = sizeof(BITMAPINFOHEADER);
	bi.biWidth = bmpDest.bmWidth;
	bi.biHeight = bmpDest.bmHeight;
	bi.biPlanes = 1;
	bi.biBitCount = 32;
	bytesPerPixel = bi.biBitCount / 8;
	bi.biCompression = BI_RGB;
	bi.biSizeImage = 0;
	bi.biXPelsPerMeter = 0;
	bi.biYPelsPerMeter = 0;
	bi.biClrUsed = 0;
	bi.biClrImportant = 0;

	pitch = bytesPerPixel * sourceWidth;

	//The size of the (dest) bitmap
	dwBmpSize = ((bmpDest.bmWidth * bi.biBitCount + 31) / 32) * 4 * bmpDest.bmHeight;

	/* Starting with 32-bit Windows, GlobalAlloc and LocalAlloc are implemented as wrapper functions that
	call HeapAlloc using a handle to the process's default heap. Therefore, GlobalAlloc and LocalAlloc
	have greater overhead than HeapAlloc. */

	//Allocate memory for the (dest) bitmap
	hDIB = GlobalAlloc(GHND, dwBmpSize);
	bitmap = (uint8_t *)GlobalLock(hDIB);
}

void WinCap::onResize() {
	// Get the client area for size calculation
	GetClientRect(source, &sourceRect);

	sourceWidth = sourceRect.right - sourceRect.left;
	sourceHeight = sourceRect.bottom - sourceRect.top;
	pitch = bytesPerPixel * sourceWidth;
}

void WinCap::updateBitmap() {
	if (IsWindowVisible(source)) {
		// Bit block transfer into our compatible memory DC.
		if (!BitBlt(hdcMemDC,
			0, 0, sourceWidth, sourceHeight,
			hdcSource,
			0, 0, SRCCOPY)) {
			MessageBoxA(0, "BitBlt has failed", "Failed", MB_OK);
			return;
		}

		/* Gets the "bits" from the bitmap and copies them into a buffer
		which is pointed to by bitmap.*/
		GetDIBits(hdcMemDC, hbmSource, 0,
			(UINT)bmpDest.bmHeight, bitmap,
			(BITMAPINFO *)&bi, DIB_RGB_COLORS);
	}
}

void WinCap::saveBitmap(const char *FILENAME) {
	BITMAPFILEHEADER   bmfHeader;

	// A file is created, this is where we will save the screen capture.
	HANDLE hFile = CreateFileA(FILENAME,
		GENERIC_WRITE,
		0,
		NULL,
		CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL, NULL);

	// Add the size of the headers to the size of the bitmap to get the total file size
	DWORD dwSizeofDIB = dwBmpSize + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

	//Offset to where the actual bitmap bits start.
	bmfHeader.bfOffBits = (DWORD)sizeof(BITMAPFILEHEADER) + (DWORD)sizeof(BITMAPINFOHEADER);

	//Size of the file
	bmfHeader.bfSize = dwSizeofDIB;

	//bfType must always be BM for Bitmaps
	bmfHeader.bfType = 0x4D42; //BM   

	DWORD dwBytesWritten = 0;

	WriteFile(hFile, (LPSTR)&bmfHeader, sizeof(BITMAPFILEHEADER), &dwBytesWritten, NULL);
	WriteFile(hFile, (LPSTR)&bi, sizeof(BITMAPINFOHEADER), &dwBytesWritten, NULL);
	WriteFile(hFile, (LPSTR)bitmap, dwBmpSize, &dwBytesWritten, NULL);

	//Close the handle for the file that was created
	CloseHandle(hFile);
}

void WinCap::flipBitmap() {
	uint32_t *pixels = (uint32_t*)bitmap;
	int width = sourceWidth;
	int heightOffset = sourceHeight - 1;
	int height = (heightOffset + 1) / 2;
	for (int x = 0; x < width; ++x) {
		for (int y = 0; y < height; ++y) {
			uint32_t colorOne = *(pixels + y * width + x);
			uint32_t colorTwo = *(pixels + (heightOffset - y) * width + x);
			*(pixels + y * width + x) = colorTwo;
			*(pixels + (heightOffset - y) * width + x) = colorOne;
		}
	}
}