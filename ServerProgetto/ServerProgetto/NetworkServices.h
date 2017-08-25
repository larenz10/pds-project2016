#pragma once

#include "stdafx.h" 
#include <Windows.h>
#include "Applicazione.h"
#include "rapidjson\writer.h"
#include "rapidjson\prettywriter.h"
#include "rapidjson\stringbuffer.h"
#include "rapidjson\document.h"

class NetworkServices
{
public:
//	static BOOL CALLBACK enumWindowsProc(HWND hWnd, LPARAM lParam);
	static DWORD BitmapToBuffer(HBITMAP hBitmap, HDC hdc, BYTE **buffer);
};
 
