#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>

// Need to link with Ws2_32.lib
#pragma comment (lib, "Ws2_32.lib")

#define DEFAULT_BUFLEN 512
//#define DEFAULT_PORT "1500"

class Server {
	WSADATA wsaData;

	SOCKET ListenSocket = INVALID_SOCKET;
	SOCKET ClientSocket = INVALID_SOCKET;

	struct addrinfo *addr = NULL;
	struct addrinfo hints;

	int iSendResult;
	char recvbuf[DEFAULT_BUFLEN];
	int recvbuflen = DEFAULT_BUFLEN;

public:
	int serverSetup(PCSTR port) {
		int result;

		// inizializazione del socket
		result = WSAStartup(0x0202, &wsaData);
		if (result != 0) {
			printf("WSAStartup fallita con errore: %d\n", result);
			return 0;
		}

		ZeroMemory(&hints, sizeof(hints));
		hints.ai_family = AF_INET;
		hints.ai_socktype = SOCK_STREAM;
		hints.ai_protocol = IPPROTO_TCP;
		hints.ai_flags = AI_PASSIVE;

		// risoluzione di indirizzo e porta del server
		result = getaddrinfo(NULL, port, &hints, &addr);
		if (result != 0) {
			printf("getaddrinfo fallita con errore: %d\n", result);
			WSACleanup();
			return 0;
		}

		// Creazione di un socket per connettere il server
		ListenSocket = socket(addr->ai_family, addr->ai_socktype, addr->ai_protocol);
		if (ListenSocket == INVALID_SOCKET) {
			printf("socket fallita con errore: %ld\n", WSAGetLastError());
			freeaddrinfo(addr);
			WSACleanup();
			return 0;
		}

		// Setup del socket TCP
		result = bind(ListenSocket, addr->ai_addr, (int)addr->ai_addrlen);
		if (result == SOCKET_ERROR) {
			printf("bind fallita con errore: %d\n", WSAGetLastError());
			freeaddrinfo(addr);
			closesocket(ListenSocket);
			WSACleanup();
			return 0;
		}

		freeaddrinfo(addr);

		result = listen(ListenSocket, SOMAXCONN);
		if (result == SOCKET_ERROR) {
			printf("listen fallita con errore: %d\n", WSAGetLastError());
			closesocket(ListenSocket);
			WSACleanup();
			return 0;
		}
		return 1;
	}


	int serverConnection(){
		// Accept del socket client
		printf_s("Server in attesa di connessione...\n");
		ClientSocket = accept(ListenSocket, NULL, NULL);
		if (ClientSocket == INVALID_SOCKET) {
			printf("accept fallita con errore: %d\n", WSAGetLastError());
			closesocket(ListenSocket);
			WSACleanup();
			return 0;
		}
	printf_s("Server connesso...\n");
	return 1;
	}


	void serverClosing(){
		// server socket non più necessario
		closesocket(ListenSocket);
		WSACleanup();
	}

	/*
	BOOL CALLBACK myWindowsProc(HWND hwnd, LPARAM lParam){

		char title[80];
		GetWindowText(hwnd, (LPWSTR) title, sizeof(title));
		printf("window titl: %s\n", title);
		return TRUE;
	}


	void getApplicationInfo() {
		EnumWindows(&myWindowsProc, NULL);
		return;
	}*/
	
};

BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam)
{
	std::cout << "fin qua bene" << std::endl;
		std::wstring wnd_title[255];// = new std::wstring[255];
		GetWindowText(hwnd, (LPWSTR)wnd_title, sizeof(wnd_title));
		std::cout << wnd_title << std::endl;
	return true; // function must return true if you want to continue enumeration
}


void getApplicationInfo() {
	EnumWindows(EnumWindowsProc, NULL);
	return;
}

int main() {
	//EnumWindows(EnumWindowsProc, NULL);
	Server myServer;
	myServer.serverSetup("1500"); 
	while (1)
	{
		myServer.serverConnection();

	}
	return 0;
}