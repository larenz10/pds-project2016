#pragma once

#include <memory>
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <Windows.h>

class Applicazione {
	std::wstring name;
	DWORD process;
	bool existIcon;
	bool coloredIcon;
	std::string bitmaskBuffer;
	std::string colorBuffer;
	bool focus;

public:
	Applicazione();
	Applicazione(std::wstring n, DWORD p);
	~Applicazione();
	std::wstring getName();
	DWORD getProcess();
	bool getExistIcon();
	void setExistIcon(bool exist);
	bool getColoredIcon();
	void setColoredIcon(bool colored);
	std::string getBitmaskBuffer();
	void setBitmaskBuffer(BYTE *buffer, DWORD nBytes);
	std::string getColorBuffer();
	void setColorBuffer(BYTE *buffer, DWORD nBytes);
	bool getFocus();
	void setFocus(bool focus);
};
