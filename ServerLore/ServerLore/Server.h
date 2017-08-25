#pragma once

#include <iostream>
#include <list>
#include <psapi.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <vector>

#include "Applicazione.h"
#include "NetworkServices.h"

#pragma comment(lib, "Ws2_32.lib")

#define DEFAULT_BUFLEN 512

class Server {
	SOCKET listenSocket;
	SOCKET clientSocket;
	int result;
	std::vector<Applicazione> apps;

public:
	Server(PCSTR port);
	~Server();
	int serverConnection();
	void serverClosing();
	void getApplicationInfo();
	std::vector<Applicazione> getApps();
	void deleteApps();
	void addApp(Applicazione app);
	template<typename Writer> void serialize(Writer &writer, Applicazione app);
	int sendApp(Applicazione app);
	bool isAltTabWindow(HWND hWnd);
};

