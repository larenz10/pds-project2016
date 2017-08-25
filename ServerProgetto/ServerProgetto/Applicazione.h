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
	bool coloredIcon;
	std::string bitmaskBuffer;
	std::string colorBuffer;
	bool focus;

public:
	Applicazione(); 
	Applicazione(std::wstring name, DWORD process);
	~Applicazione();
	std::wstring getName();
	DWORD getProcess();
	bool getFocus();
	std::string getBitmaskBuffer();
	std::string getColorBuffer();
	bool getExistIcon();
	bool getColoredIcon();
	void setFocus(bool focus);
	void setBitmaskBuffer(BYTE *buffer, DWORD nBytes);
	void setColorBuffer(BYTE *buffer, DWORD nBytes);
	void setExistIcon(bool exist); 
	void setColoredIcon(bool colored);
};

