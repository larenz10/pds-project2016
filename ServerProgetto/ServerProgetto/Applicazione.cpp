#include "stdafx.h"
#include "Applicazione.h"

Applicazione::Applicazione()
{
}

Applicazione::~Applicazione()
{
}

Applicazione::Applicazione(std::wstring name, DWORD process) {
	this->name = name;
	this->process = process;

	this->focus = false;
}

void Applicazione::setIcon(bool icon) {
	this->existIcon = icon;
}

void Applicazione::setFocus(bool focus) {
	this->focus = focus;
}

void Applicazione::setBitmapBuf(char *buffer, BITMAPINFO *bi, int bmpSize) {
	int i = 2;
	/* copio le dati e info della bitmap */
	char *buf = (char*)malloc(2 + sizeof(BITMAPINFO) + bmpSize);
	memcpy(buf + i, bi, sizeof(BITMAPINFO));
	i += sizeof(BITMAPINFO);
	memcpy(buf + i, buffer, bmpSize);
	i += bmpSize;
	/* memorizzo dimensioni del buffer da inviare(dovranno poi essere moltiplicate per 256) */
	buf[0] = (i / 256);
	buf[1] = (i % 256);

	bitmapBuf.resize(2 + sizeof(BITMAPINFO) + bmpSize);
	bitmapBuf.assign(buf);

	free(buf);
	buf = NULL;
}

std::wstring Applicazione::getName() {
	return this->name;
}

DWORD Applicazione::getProcess() {
	return this->process;
}

bool Applicazione::getFocus() {
	return this->focus;
}

std::string Applicazione::getBitmapBuf() {
	if (this->getIcon())
		return this->bitmapBuf;
	else
		return NULL;
}

bool Applicazione::getIcon() {
	return this->existIcon;
}