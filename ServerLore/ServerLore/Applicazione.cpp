#pragma once

#include "Applicazione.h"

Applicazione::Applicazione()
{
}

Applicazione::Applicazione(std::wstring n, DWORD p)
{
	name = n;
	process = p;
	focus = false;
	existIcon = false;
	coloredIcon = false;
}

Applicazione::~Applicazione()
{
}

std::wstring Applicazione::getName()
{
	return name;
}

DWORD Applicazione::getProcess()
{
	return process;
}

bool Applicazione::getExistIcon()
{
	return existIcon;
}

void Applicazione::setExistIcon(bool exist)
{
	existIcon = exist;
}

bool Applicazione::getColoredIcon()
{
	return coloredIcon;
}

void Applicazione::setColoredIcon(bool colored)
{
	coloredIcon = colored;
}

std::string Applicazione::getBitmaskBuffer()
{
	return bitmaskBuffer;
}

void Applicazione::setBitmaskBuffer(BYTE * buffer, DWORD nBytes)
{
	//Da implementare
}

std::string Applicazione::getColorBuffer()
{
	return colorBuffer;
}

void Applicazione::setColorBuffer(BYTE * buffer, DWORD nBytes)
{
	//Da implementare
}

bool Applicazione::getFocus()
{
	return focus;
}

void Applicazione::setFocus(bool focus)
{
	this->focus = focus;
}
