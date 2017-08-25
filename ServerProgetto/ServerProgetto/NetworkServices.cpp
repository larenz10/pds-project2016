#include "stdafx.h"
#include "NetworkServices.h"

//BOOL CALLBACK NetworkServices::enumWindowsProc(HWND hWnd, LPARAM lParam) {
//	BOOL focus = false;
//	std::wstring nomeApp;
//	DWORD pid;
//	HICON hIcon;
//	HBITMAP hBitmap = NULL;
//	BITMAP bitmap;
//
//	Server *thisServer = reinterpret_cast<Server *>(lParam);
//
//	/* leggo le info dell'applicazione solo se è dotata di interfaccia grafica */
//	if (IsWindowVisible(hWnd) != 0) {	/*	c'è interfaccia grafica	*/
//		const int bufferLength = GetWindowTextLength(hWnd) + 1; /* lunghezza della stringa di testo */
//		nomeApp.resize(bufferLength); /* alloco stringa di lunghezza appropriata */
//		int textResult = GetWindowText(hWnd, &nomeApp[0], bufferLength);
//		nomeApp.resize(bufferLength - 1); /* resize per eliminare eventuali ambigui doppi valori NULL di terminazione */
//
//		if (textResult != 0) {
//			GetWindowThreadProcessId(hWnd, &pid);	/*	processo che gestisce l'applicazione */
//
//			std::wcout << L"nome applicazione : " << nomeApp.c_str() << std::endl;
//			std::cout << "pid applicazione: " << pid << std::endl;
//
//			Applicazione app(nomeApp, pid);
//
//			hIcon = (HICON)GetClassLongPtr(hWnd, GCLP_HICON); /* NULL nel caso l'applicazione non abbia icona */
//			if (hIcon != NULL) { /* l'applicazione ha icona */
//				hBitmap = iconToBitmap(hIcon);
//
//				// ottengo la BITMAP da HBITMAP
//				GetObject(hBitmap, sizeof(BITMAP), &bitmap); // fino a qui ok!
//
//				int cClrBits = bitmap.bmPlanes*bitmap.bmBitsPixel;
//
//				BITMAPINFO bi;
//				memset(&bi, 0, sizeof(BITMAPINFO));
//
//				bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
//				bi.bmiHeader.biWidth = bitmap.bmWidth;
//				bi.bmiHeader.biHeight = bitmap.bmHeight;
//				bi.bmiHeader.biPlanes = bitmap.bmPlanes;
//				bi.bmiHeader.biBitCount = bitmap.bmBitsPixel;
//				bi.bmiHeader.biCompression = BI_RGB;
//				//				bi.bmiHeader.biSizeImage = ((bi.bmiHeader.biWidth * cClrBits + 31) & ~31) / 8 * bi.bmiHeader.biHeight;  commentato perchè deve essere zero se si usa BI_RGB
//				if (cClrBits<24)
//				{
//					bi.bmiHeader.biClrUsed = (1 << cClrBits);
//				}
//
//				DWORD dwBmpSize = ((bitmap.bmWidth * bi.bmiHeader.biBitCount + 31) / 32) * 4 * bitmap.bmHeight;
//
//				// Starting with 32-bit Windows, GlobalAlloc and LocalAlloc are implemented as wrapper functions that 
//				// call HeapAlloc using a handle to the process's default heap. Therefore, GlobalAlloc and LocalAlloc 
//				// have greater overhead than HeapAlloc.
//				HANDLE hDIB = GlobalAlloc(GHND, dwBmpSize); /* ottengo memoria dallo heap */
//				char *buf = (char *)GlobalLock(hDIB); /* ottengo il puntatore alla memoria allocata precedentemente */
//
//				GetDIBits(GetDC(NULL), hBitmap, 0, (UINT)bitmap.bmHeight, buf, &bi, DIB_RGB_COLORS);
//
//				app.setIcon(true);
//				std::cout << "icona trovata" << std::endl;
//
//				app.setBitmapBuf(buf, &bi, dwBmpSize);
//
//				//tolgo il blocco e libero il DIB dallo heap
//				GlobalUnlock(hDIB);
//				GlobalFree(hDIB);
//
//				DeleteObject(hBitmap);
//			}
//			else { /* l'applicazione non ha icona */
//				app.setIcon(false);
//			}
//
//			if (hWnd == GetForegroundWindow()) /* l'applicazione considerata è quella con focus */
//				app.setFocus(true);
//
//			thisServer->addApp(app);
//		}
//	}
//	return true;
//}

DWORD NetworkServices::BitmapToBuffer(HBITMAP hBitmap, HDC hdc, BYTE **buffer) {

	PBITMAPINFO pbi;
	WORD cClrBits = 0;
	int res;
	BITMAP bitMask;

	// Convert the color format to a count of bits.  
	GetObject(hBitmap, sizeof(BITMAP), (LPSTR)&bitMask);
	cClrBits = (WORD)(bitMask.bmPlanes * bitMask.bmBitsPixel);
	if (cClrBits == 1)
		cClrBits = 1;
	else if (cClrBits <= 4)
		cClrBits = 4;
	else if (cClrBits <= 8)
		cClrBits = 8;
	else if (cClrBits <= 16)
		cClrBits = 16;
	else if (cClrBits <= 24)
		cClrBits = 24;
	else cClrBits = 32;

	// Allocate memory for the BITMAPINFO structure. (This structure  
	// contains a BITMAPINFOHEADER structure and an array of RGBQUAD  
	// data structures.)  

	if (cClrBits < 24)
		pbi = (PBITMAPINFO)malloc(sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD) * (1 << cClrBits));
	// There is no RGBQUAD array for these formats: 24-bit-per-pixel or 32-bit-per-pixel 
	else
		pbi = (PBITMAPINFO)malloc(sizeof(BITMAPINFOHEADER));

	// Initialize the fields in the BITMAPINFO structure. 
	pbi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	pbi->bmiHeader.biWidth = bitMask.bmWidth;
	pbi->bmiHeader.biHeight = bitMask.bmHeight;
	pbi->bmiHeader.biPlanes = bitMask.bmPlanes;
	pbi->bmiHeader.biBitCount = bitMask.bmBitsPixel;
	if (cClrBits < 24)
		pbi->bmiHeader.biClrUsed = (1 << cClrBits);
	pbi->bmiHeader.biCompression = BI_RGB; // If the bitmap is not compressed, set the BI_RGB flag. 
										   // Compute the number of bytes in the array of color indices and store the result in biSizeImage.  
										   // The width must be DWORD aligned unless the bitmap is RLE compressed. 
	pbi->bmiHeader.biSizeImage = ((pbi->bmiHeader.biWidth * cClrBits + 31) & ~31) / 8 * pbi->bmiHeader.biHeight;
	// Set biClrImportant to 0, indicating that all of the device colors are important.  
	pbi->bmiHeader.biClrImportant = 0;
	// create the bitmap buffer
	*buffer = (BYTE*)malloc(pbi->bmiHeader.biSizeImage);
	DeleteObject(&bitMask);
	// get the actual bitmap buffer
	if (0 == GetDIBits(hdc, (HBITMAP)hBitmap, 0, (WORD)pbi->bmiHeader.biHeight, *buffer, pbi, DIB_RGB_COLORS))
		return (DWORD)0;
	else
		return pbi->bmiHeader.biSizeImage;
	}