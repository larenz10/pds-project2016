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

HBITMAP NetworkServices::iconToBitmap(HICON hIcon) {
	HDC hDC = GetDC(NULL);
	HDC hMemDC = CreateCompatibleDC(hDC);
	int x = GetSystemMetrics(SM_CXICON);
	int y = GetSystemMetrics(SM_CYICON);
	HBITMAP hMemBmp = CreateCompatibleBitmap(hDC, x, y);
	HBITMAP hResultBmp = NULL;
	HGDIOBJ hOrgBMP = SelectObject(hMemDC, hMemBmp);

	DrawIconEx(hMemDC, 0, 0, hIcon, x, y, 0, NULL, DI_NORMAL);

	hResultBmp = hMemBmp;
	hMemBmp = NULL;

	SelectObject(hMemDC, hOrgBMP);
	DeleteDC(hMemDC);
	ReleaseDC(NULL, hDC);
	DestroyIcon(hIcon);
	return hResultBmp;
}
