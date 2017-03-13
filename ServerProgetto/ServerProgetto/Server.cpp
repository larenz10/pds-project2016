#include "stdafx.h"
#include "Server.h"

BOOL CALLBACK enumWindowsProc(HWND hWnd, LPARAM lParam) {
	BOOL focus = false;
	std::wstring nomeApp;
	DWORD pid;
	HICON hIcon;
	HBITMAP hBitmap = NULL;
	BITMAP bitmap;

	Server *thisServer = reinterpret_cast<Server *>(lParam);

	/* leggo le info dell'applicazione solo se è dotata di interfaccia grafica */
	if (IsWindowVisible(hWnd) != 0) {	/*	c'è interfaccia grafica	*/
		const int bufferLength = GetWindowTextLength(hWnd) + 1; /* lunghezza della stringa di testo */
		nomeApp.resize(bufferLength); /* alloco stringa di lunghezza appropriata */
		int textResult = GetWindowText(hWnd, &nomeApp[0], bufferLength);
		nomeApp.resize(bufferLength - 1); /* resize per eliminare eventuali ambigui doppi valori NULL di terminazione */

		if (textResult != 0) {
			GetWindowThreadProcessId(hWnd, &pid);	/*	processo che gestisce l'applicazione */

			std::wcout << L"nome applicazione : " << nomeApp.c_str() << std::endl;
			std::cout << "pid applicazione: " << pid << std::endl;

			Applicazione app(nomeApp, pid);

			hIcon = (HICON)GetClassLongPtr(hWnd, GCLP_HICON); /* NULL nel caso l'applicazione non abbia icona */
			if (hIcon != NULL) { /* l'applicazione ha icona */
				hBitmap = NetworkServices::iconToBitmap(hIcon);

				// ottengo la BITMAP da HBITMAP
				GetObject(hBitmap, sizeof(BITMAP), &bitmap); // fino a qui ok!

				int cClrBits = bitmap.bmPlanes*bitmap.bmBitsPixel;

				BITMAPINFO bi;
				memset(&bi, 0, sizeof(BITMAPINFO));

				bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
				bi.bmiHeader.biWidth = bitmap.bmWidth;
				bi.bmiHeader.biHeight = bitmap.bmHeight;
				bi.bmiHeader.biPlanes = bitmap.bmPlanes;
				bi.bmiHeader.biBitCount = bitmap.bmBitsPixel;
				bi.bmiHeader.biCompression = BI_RGB;
				//				bi.bmiHeader.biSizeImage = ((bi.bmiHeader.biWidth * cClrBits + 31) & ~31) / 8 * bi.bmiHeader.biHeight;  commentato perchè deve essere zero se si usa BI_RGB
				if (cClrBits<24)
				{
					bi.bmiHeader.biClrUsed = (1 << cClrBits);
				}

				DWORD dwBmpSize = ((bitmap.bmWidth * bi.bmiHeader.biBitCount + 31) / 32) * 4 * bitmap.bmHeight;

				// Starting with 32-bit Windows, GlobalAlloc and LocalAlloc are implemented as wrapper functions that 
				// call HeapAlloc using a handle to the process's default heap. Therefore, GlobalAlloc and LocalAlloc 
				// have greater overhead than HeapAlloc.
				HANDLE hDIB = GlobalAlloc(GHND, dwBmpSize); /* ottengo memoria dallo heap */
				char *buf = (char *)GlobalLock(hDIB); /* ottengo il puntatore alla memoria allocata precedentemente */

				GetDIBits(GetDC(NULL), hBitmap, 0, (UINT)bitmap.bmHeight, buf, &bi, DIB_RGB_COLORS);

				app.setIcon(true);
				std::cout << "icona trovata" << std::endl;

				app.setBitmapBuf(buf, &bi, dwBmpSize);

				//tolgo il blocco e libero il DIB dallo heap
				GlobalUnlock(hDIB);
				GlobalFree(hDIB);

				DeleteObject(hBitmap);
			}
			else { /* l'applicazione non ha icona */
				app.setIcon(false);
			}

			if (hWnd == GetForegroundWindow()) /* l'applicazione considerata è quella con focus */
				app.setFocus(true);

			thisServer->addApp(app); 
		}
	}
	return true;
}

Server::Server(PCSTR port)
{
	WSADATA wsaData;

	ListenSocket = INVALID_SOCKET;
	ClientSocket = INVALID_SOCKET;

	struct addrinfo *addr = NULL;
	struct addrinfo hints;

	// inizializazione del socket
	result = WSAStartup(0x0202, &wsaData);
	if (result != 0) {
		printf("WSAStartup fallita con errore: %d\n", result);
		exit(1);
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
		exit(1);
	}

	// Creazione di un socket per connettere il server
	ListenSocket = socket(addr->ai_family, addr->ai_socktype, addr->ai_protocol);
	if (ListenSocket == INVALID_SOCKET) {
		printf("socket fallita con errore: %ld\n", WSAGetLastError());
		freeaddrinfo(addr);
		WSACleanup();
		exit(1);
	}

	// Setup del socket TCP
	result = bind(ListenSocket, addr->ai_addr, (int)addr->ai_addrlen);
	if (result == SOCKET_ERROR) {
		printf("bind fallita con errore: %d\n", WSAGetLastError());
		freeaddrinfo(addr);
		closesocket(ListenSocket);
		WSACleanup();
		exit(1);
	}

	freeaddrinfo(addr);

	result = listen(ListenSocket, SOMAXCONN);
	if (result == SOCKET_ERROR) {
		printf("listen fallita con errore: %d\n", WSAGetLastError());
		closesocket(ListenSocket);
		WSACleanup();
		exit(1);
	}
}

int Server::serverConnection() {
	// Accept del socket client
	std::cout << "Server in attesa di connessione..." << std::endl;
	ClientSocket = accept(ListenSocket, NULL, NULL);
	if (ClientSocket == INVALID_SOCKET) {
		std::cout << "accept fallita con errore: " << WSAGetLastError() << std::endl;
		closesocket(ListenSocket);
		WSACleanup();
		return 0;
	}
	std::cout << "Server connesso..." << std::endl;
	return 1;
}

void Server::serverClosing() {
	// server socket non più necessario
	closesocket(ListenSocket);
	WSACleanup();
}

void Server::getApplicationInfo() {
	EnumWindows(enumWindowsProc, (LPARAM)this);
	return;
}

void Server::addApp(Applicazione a) {
	this->applicazioni.push_back(a);
}

template <typename Writer> void Server::serialize(Writer &writer, Applicazione a) {
	std::string str;
	for (char c : a.getName())	/* pensare a una conversione wstring -> string più intelligente */
		str += c;

	writer.StartObject();

	writer.String("name");
	writer.String(str.c_str(), str.size());
	writer.String("process");
	writer.Uint(a.getProcess());
	writer.String("existIcon");
	writer.Bool(a.getIcon());
	writer.String("bitmapBuf");
	if (a.getIcon())
		writer.String(a.getBitmapBuf().c_str(), a.getBitmapBuf().size());
	else
		writer.Null();
	writer.String("focus");
	writer.Bool(a.getFocus());

	writer.EndObject();
}

int Server::sendApp(Applicazione app) {
//	std::vector<rapidjson::StringBuffer> sbVector;
	rapidjson::StringBuffer sb;
	rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(sb);

	serialize(writer, app);
	sb.Push(sb.GetSize());

	result = send(ClientSocket, sb.GetString(), sb.GetSize(), 0);
	if (result != sb.GetSize())
		std::cout << "errore nell'invio dei dati..." << std::endl;
	else
		std::cout << "dati inviati con successo!" << std::endl;

	sb.Clear();
	writer.Reset(sb);
	return result;
}

std::vector<Applicazione> Server::getApplicazioni() {
	return applicazioni;
}

Server::~Server()
{
}
