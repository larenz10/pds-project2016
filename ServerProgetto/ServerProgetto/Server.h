#pragma once

#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <list>
#include <vector>
#include <sstream>
#include "NetworkServices.h"
#include "Applicazione.h"


// Need to link with Ws2_32.lib
#pragma comment (lib, "Ws2_32.lib")

#define DEFAULT_BUFLEN 512
/* #define DEFAULT_PORT "1500"  decommentare nel caso si voglia impostare una default port in questo modo */

class Server
{
	SOCKET ListenSocket;
	SOCKET ClientSocket;
	int result; /* per fare check delle operazioni */
	std::vector<Applicazione> applicazioni;

public:
	Server(PCSTR port);
	~Server();
	int serverConnection();
	void serverClosing();
	void getApplicationInfo();
	std::vector<Applicazione> getApplicazioni();
	void addApp(Applicazione app);
	template <typename Writer> void serialize(Writer &writer, Applicazione app);
	int sendApp(Applicazione app);
};

