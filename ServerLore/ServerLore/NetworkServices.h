#pragma once

#include "Applicazione.h"

class NetworkServices {
public:
	static DWORD bitmapToBuffer(HBITMAP hBitmap, HDC hdc, BYTE *buffer);
};