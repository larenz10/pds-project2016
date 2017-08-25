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
	this->existIcon = false;
	this->coloredIcon = false;
}

void Applicazione::setExistIcon(bool exist) {
	this->existIcon = exist;
}

void Applicazione::setColoredIcon(bool colored) {
	this->coloredIcon = colored;
}

void Applicazione::setFocus(bool focus) {
	this->focus = focus;
}

void Applicazione::setBitmaskBuffer(BYTE *buffer, DWORD nBytes) {
	bitmaskBuffer.resize(nBytes);
	for (int i = 0; i < nBytes; i++)
		bitmaskBuffer[i] = buffer[i];
	return;
}

void Applicazione::setColorBuffer(BYTE *buffer, DWORD nBytes) {
	colorBuffer.resize(nBytes);
	for (int i = 0; i < nBytes; i++)
		colorBuffer[i] = buffer[i];
	//memcpy(&colorBuffer, buffer, nBytes);
	return;
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

std::string Applicazione::getBitmaskBuffer() {
	if (this->getExistIcon())
		return this->bitmaskBuffer;
	else
		return NULL;
}

std::string Applicazione::getColorBuffer() {
	if (this->getColoredIcon())
		return this->colorBuffer;
	else
		return NULL;
}

bool Applicazione::getExistIcon() {
	return this->existIcon;
}

bool Applicazione::getColoredIcon() {
	return this->coloredIcon;
}