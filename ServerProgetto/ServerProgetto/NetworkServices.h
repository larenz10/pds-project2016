#pragma once

#include "stdafx.h" 
#include <Windows.h>
#include "Applicazione.h"
#include "rapidjson\writer.h"
#include "rapidjson\prettywriter.h"
#include "rapidjson\stringbuffer.h"

class NetworkServices
{
public:
//	static BOOL CALLBACK enumWindowsProc(HWND hWnd, LPARAM lParam);
	static HBITMAP iconToBitmap(HICON hIcon);
};

