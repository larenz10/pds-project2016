#pragma once

#include <memory>
#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <string>

class Applicazione
{
	//	Server* server;		serve? pensare di settarlo lato client
	std::wstring name;
	DWORD process;
	bool existIcon;
	std::string bitmapBuf;
	bool focus;

public:
	Applicazione(); 
	Applicazione(std::wstring name, DWORD process);
	~Applicazione();
	std::wstring getName();
	DWORD getProcess();
	bool getFocus();
	std::string getBitmapBuf();
	bool getIcon();
	void setFocus(bool focus);
	void setBitmapBuf(char *buf, BITMAPINFO *bi, int bmpSize);
	void setIcon(bool icona);
};

