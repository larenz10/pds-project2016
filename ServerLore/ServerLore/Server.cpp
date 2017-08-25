#include "Server.h"
#include "stdafx.h"

Server::Server(PCSTR port)
{
	WSADATA wsaData;

	listenSocket = INVALID_SOCKET;
	clientSocket = INVALID_SOCKET;

	struct addrinfo *addr = NULL;
	struct addrinfo hints;

	//Inizializzazione del socket
	result = WSAStartup(0x0202, &wsaData);
	if (result != 0) {
		printf("WSAStartup fallita con errore %d.\n", result);
		exit(1);
	}

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	result = getaddrinfo(NULL, port, &hints, &addr);
	if (result != 0) {
		printf("getaddrinfo fallita con errore %d.\n", result);
		WSACleanup();
		exit(1);
	}

	listenSocket = socket(addr->ai_family, addr->ai_socktype, addr->ai_protocol);
	if (listenSocket == INVALID_SOCKET) {
		printf("socket fallita con errore %ld.\n", WSAGetLastError());
		freeaddrinfo(addr);
		WSACleanup();
		exit(1);
	}

	result = bind(listenSocket, addr->ai_addr, (int)addr->ai_addrlen);
	if (result == SOCKET_ERROR) {
		printf("bind fallita con errore %ld.\n", WSAGetLastError());
		freeaddrinfo(addr);
		closesocket(listenSocket);
		WSACleanup();
		exit(1);
	}
	freeaddrinfo(addr);

	result = listen(listenSocket, SOMAXCONN);
	if (result == SOCKET_ERROR) {
		printf("listen fallita con errore %ld.\n", WSAGetLastError());
		closesocket(listenSocket);
		WSACleanup();
		exit(1);
	}
}

Server::~Server()
{
}

int Server::serverConnection()
{
	std::cout << "Server in attesa di connessione..." << std::endl;
	clientSocket = accept(listenSocket, NULL, NULL);
	if (clientSocket == INVALID_SOCKET) {
		std::cout << "accept fallita con errore %ld.\n", WSAGetLastError();
		closesocket(listenSocket);
		WSACleanup();
		return 0;
	}
	std::cout << "Server connesso!" << std::endl;
	return 1;
}

void Server::serverClosing()
{
	closesocket(listenSocket);
	WSACleanup();
}

void Server::getApplicationInfo()
{
	//Da implementare
}

std::vector<Applicazione> Server::getApps()
{
	return apps;
}

void Server::deleteApps()
{
	apps.clear();
}

void Server::addApp(Applicazione app)
{
	apps.push_back(app);
}

template <typename Writer> void Server::serialize(Writer &writer, Applicazione a)
{
	//Da implementare
}

int Server::sendApp(Applicazione app)
{
	//Da implementare
	return 0;
}

bool Server::isAltTabWindow(HWND hWnd)
{
	TITLEBARINFO ti;
	HWND hwndTry, hwndWalk = NULL;
	if (!IsWindowVisible(hWnd))
		return false;
	hwndTry = GetAncestor(hWnd, GA_ROOTOWNER);
	while (hwndTry != hwndWalk) {
		hwndWalk = hwndTry;
		hwndTry = GetLastActivePopup(hwndWalk);
		if (IsWindowVisible(hwndTry))
			break;
	}
	if (hwndWalk != hWnd)
		return false;
	// the following removes some task tray programs and "Program Manager"
	ti.cbSize = sizeof(ti);
	GetTitleBarInfo(hWnd, &ti);
	if (ti.rgstate[0] & STATE_SYSTEM_INVISIBLE)
		return FALSE;

	// Tool windows should not be displayed either, these do not appear in the
	// task bar.
	if (GetWindowLong(hWnd, GWL_EXSTYLE) & WS_EX_TOOLWINDOW)
		return FALSE;

	return TRUE;
}