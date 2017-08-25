#include "stdafx.h"
#include "Server.h"

BOOL CALLBACK enumWindowsProc(HWND hWnd, LPARAM lParam) {
	BOOL focus = false;
	std::wstring nomeApp, pathApp;
	DWORD pid, nBytes;
	HANDLE hProcess;
	HDC hdc;
	Server *thisServer = reinterpret_cast<Server *>(lParam);



	if (thisServer->IsAltTabWindow(hWnd)) {
		// la finestra ha un'interfaccia grafica

		const int bufferLength = GetWindowTextLength(hWnd) + 1; /* lunghezza della stringa di testo */
		nomeApp.resize(bufferLength); /* alloco stringa di lunghezza appropriata */
		int textResult = GetWindowText(hWnd, &nomeApp[0], bufferLength);
		nomeApp.resize(bufferLength - 1);
		GetWindowThreadProcessId(hWnd, &pid);	/*	processo che gestisce l'applicazione */
		Applicazione app(nomeApp, pid);
		hdc = GetDC(hWnd);
		// Create a compatible DC which is used in a BitBlt from the window DC
		pathApp.resize(MAX_PATH);
		hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
		if (hProcess != NULL) {
			HICON hIcon;

			textResult = GetModuleFileNameEx(hProcess, NULL, &pathApp[0], MAX_PATH);
			pathApp.resize(textResult);
			if ((hIcon = (HICON)GetClassLong(hWnd, GCL_HICON)) != NULL) {
				// è presente un'icona
				ICONINFO iconInfo = { 0 };
				BYTE *maskBuffer = NULL;
				BITMAP bitMask, bitColor;

				// Retrieve the bitmap color format, width, and height. 
				GetIconInfo(hIcon, &iconInfo);
				//GetObject(iconInfo.hbmMask, sizeof(BITMAP), (LPSTR)&bitMask);
				nBytes = NetworkServices::BitmapToBuffer(iconInfo.hbmMask, hdc, &maskBuffer);
				if (nBytes <= 0) {
					app.setExistIcon(false);
				}
				else {
					app.setExistIcon(true);
					app.setBitmaskBuffer(maskBuffer, nBytes);
					free(maskBuffer);
					maskBuffer = NULL;
					//					DeleteObject(&bitMask);
					//					GetObject(iconInfo.hbmColor, sizeof(BITMAP), (LPSTR)&bitColor);
					if (iconInfo.hbmColor == NULL)
						app.setColoredIcon(false);
					else {
						nBytes = NetworkServices::BitmapToBuffer(iconInfo.hbmColor, hdc, &maskBuffer);
						if (nBytes <= 0)
							app.setColoredIcon(false);
						else {
							app.setColoredIcon(true);
							app.setColorBuffer(maskBuffer, nBytes);
						}
						if (maskBuffer != NULL)
							free(maskBuffer);
						maskBuffer = NULL;
						//					DeleteObject(&bitColor);
					}
				}
				//Clean up
				DestroyIcon(hIcon);
			}
		}
		if (hWnd == GetForegroundWindow()) /* l'applicazione considerata è quella con focus */
			app.setFocus(true);

		thisServer->addApp(app);
		
		ReleaseDC(hWnd, hdc);
	}
	return true;
}
//
//
//	//da qui parte vecchia
//
//	/* leggo le info dell'applicazione solo se è dotata di interfaccia grafica */
//	if (IsWindowVisible(hWnd) != 0) {	/*	c'è interfaccia grafica	*/
//		const int bufferLength = GetWindowTextLength(hWnd) + 1; /* lunghezza della stringa di testo */
//		nomeApp.resize(bufferLength); /* alloco stringa di lunghezza appropriata */
//		int textResult = GetWindowText(hWnd, &nomeApp[0], bufferLength);
//		nomeApp.resize(bufferLength - 1); /* resize per eliminare eventuali ambigui doppi valori NULL di terminazione */
//
//		if (textResult != 0) {
//			GetWindowThreadProcessId(hWnd, &pid);	/*	processo che gestisce l'applicazione */
//
//			std::wcout << L"nome applicazione : " << nomeApp.c_str() << std::endl;
//			std::cout << "pid applicazione: " << pid << std::endl;
//
//			Applicazione app(nomeApp, pid);
//
//			hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
//			if (hProcess != NULL) {
//				HMODULE hMod;
//				DWORD cbNeeded;
//				BITMAPINFO bi = {0};
//				int cClrBits;
//				HDC hdc = GetDC(hWnd);
//
//				if (EnumProcessModules(hProcess, &hMod, sizeof(hMod), &cbNeeded)) {
//					if ((hIcon = (HICON)GetClassLong(hWnd, GCL_HICON)) != NULL) {
////					if ((hIcon = ExtractIcon(GetModuleHandle(NULL), &nomeApp[0], 0)) != NULL) {
//						GetIconInfo(hIcon, &iconInfo);
//
//						/*GetObject(iconInfo.hbmMask, sizeof(BITMAP), &bitMask);
//						int maskSize = bitMask.bmWidth * bitMask.bmHeight * bitMask.bmBitsPixel / 8;
//						cClrBits = bitMask.bmPlanes*bitMask.bmBitsPixel;*/
////						memset(&bi, 0, sizeof(BITMAPINFO));
//						bi.bmiHeader.biSize = sizeof(bi.bmiHeader);
//						// Get the BITMAPINFO structure from the bitmap
//						if (0 == GetDIBits(hdc, iconInfo.hbmMask, 0, 0, NULL, &bi, DIB_RGB_COLORS)) {
//							std::cout << "error in the bitmask read" << std::endl;
//							return false;
//						}
//						// create the bitmap buffer
//						maskBuffer = new BYTE[bi.bmiHeader.biSizeImage];
//						bi.bmiHeader.biCompression = BI_RGB;
//						// get the actual bitmap buffer
//						int i = GetDIBits(hdc, iconInfo.hbmMask, 0, bi.bmiHeader.biHeight, (LPVOID)maskBuffer, &bi, DIB_RGB_COLORS);
//						if (i == 0) {
//							std::cout << "error in the second bitmask read" << std::endl;
//							return false;
//						}
//						for (int i = 0; i < 100; i++) {
//							std::cout << (int)maskBuffer[i];
//						}
//
//						ReleaseDC(NULL, hdc);
//						delete[] maskBuffer;
//					}
//				}
//			}
//
//			//hIcon = (HICON)GetClassLongPtr(hWnd, GCLP_HICON); /* NULL nel caso l'applicazione non abbia icona */
//			//if (hIcon != NULL) { /* l'applicazione ha icona */
//			//	hBitmap = NetworkServices::iconToBitmap(hIcon);
//
//			//	// ottengo la BITMAP da HBITMAP
//			//	GetObject(hBitmap, sizeof(BITMAP), &bitmap); // fino a qui ok!
//
//			//	int cClrBits = bitmap.bmPlanes*bitmap.bmBitsPixel;
//
//			//	BITMAPINFO bi;
//			//	memset(&bi, 0, sizeof(BITMAPINFO));
//
//			//	bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
//			//	bi.bmiHeader.biWidth = bitmap.bmWidth;
//			//	bi.bmiHeader.biHeight = bitmap.bmHeight;
//			//	bi.bmiHeader.biPlanes = bitmap.bmPlanes;
//			//	bi.bmiHeader.biBitCount = bitmap.bmBitsPixel;
//			//	bi.bmiHeader.biCompression = BI_RGB;
//			//	//				bi.bmiHeader.biSizeImage = ((bi.bmiHeader.biWidth * cClrBits + 31) & ~31) / 8 * bi.bmiHeader.biHeight;  commentato perchè deve essere zero se si usa BI_RGB
//			//	if (cClrBits<24)
//			//	{
//			//		bi.bmiHeader.biClrUsed = (1 << cClrBits);
//			//	}
//
//			//	DWORD dwBmpSize = ((bitmap.bmWidth * bi.bmiHeader.biBitCount + 31) / 32) * 4 * bitmap.bmHeight;
//
//			//	// Starting with 32-bit Windows, GlobalAlloc and LocalAlloc are implemented as wrapper functions that 
//			//	// call HeapAlloc using a handle to the process's default heap. Therefore, GlobalAlloc and LocalAlloc 
//			//	// have greater overhead than HeapAlloc.
//			//	HANDLE hDIB = GlobalAlloc(GHND, dwBmpSize); /* ottengo memoria dallo heap */
//			//	char *buf = (char *)GlobalLock(hDIB); /* ottengo il puntatore alla memoria allocata precedentemente */
//
//			//	GetDIBits(GetDC(NULL), hBitmap, 0, (UINT)bitmap.bmHeight, buf, &bi, DIB_RGB_COLORS);
//
//			//	app.setIcon(true);
//			//	std::cout << "icona trovata" << std::endl;
//
//			//	app.setBitmapBuf(buf, &bi, dwBmpSize);
//
//			//	//tolgo il blocco e libero il DIB dallo heap
//			//	GlobalUnlock(hDIB);
//			//	GlobalFree(hDIB);
//
//			//	DeleteObject(hBitmap);
//			//}
//			//else { /* l'applicazione non ha icona */
//			//	app.setIcon(false);
//			//}
//
//			if (hWnd == GetForegroundWindow()) /* l'applicazione considerata è quella con focus */
//				app.setFocus(true);
//
////			thisServer->addApp(app); 
//		}
//	}


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

	writer.String("Name");
	writer.String(str.c_str(), str.size());
	writer.String("Process");
	writer.Uint(a.getProcess());
	writer.String("BitmaskBuffer");
	if (a.getExistIcon()) {
		writer.String(a.getBitmaskBuffer().c_str(), a.getBitmaskBuffer().size());
		writer.String("ColorBuffer");
		if(a.getColoredIcon())
			writer.String(a.getColorBuffer().c_str(), a.getColorBuffer().size());
		else
			writer.Null();
	}		
	else
		writer.Null();
	writer.String("Focus");
	writer.Bool(a.getFocus());

	writer.EndObject();
}

int Server::sendApp(Applicazione app) {
	rapidjson::StringBuffer sb;
	rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(sb);
	char code = 'a';
	/* serializzazione dei dati tramite rapidjson */
	serialize(writer, app);

	UINT32 bufferSize = static_cast<UINT32>(sb.GetSize());
	std::vector<unsigned char> buffer;
	buffer.resize(sizeof(bufferSize) + sb.GetSize() + 1);
	memcpy(&buffer[0], &bufferSize, sizeof(bufferSize));
	memcpy(&buffer[4], &code, sizeof(char));
	memcpy(&buffer[5], sb.GetString(), sb.GetSize());
	std::string str(buffer.begin(), buffer.end());
	result = send(ClientSocket, str.c_str(), buffer.size(), 0);
	if (result != buffer.size())
		std::cout << "errore nell'invio dei dati..."<< std::endl;
	else
		std::cout << "dati inviati con successo!" << std::endl;

	/* pulisco il buffer e stringa */
	sb.Clear();
	writer.Reset(sb);
	str.clear();

	return result;
}

std::vector<Applicazione> Server::getApplicazioni() {
	return applicazioni;
}


void Server::deleteApplicazioni() {
	applicazioni.clear();
}


bool Server::IsAltTabWindow(HWND hwnd){
	TITLEBARINFO ti;
	HWND hwndTry, hwndWalk = NULL;

	if (!IsWindowVisible(hwnd))
		return FALSE;

	hwndTry = GetAncestor(hwnd, GA_ROOTOWNER);
	while (hwndTry != hwndWalk)
	{
		hwndWalk = hwndTry;
		hwndTry = GetLastActivePopup(hwndWalk);
		if (IsWindowVisible(hwndTry))
			break;
	}
	if (hwndWalk != hwnd)
		return FALSE;

	// the following removes some task tray programs and "Program Manager"
	ti.cbSize = sizeof(ti);
	GetTitleBarInfo(hwnd, &ti);
	if (ti.rgstate[0] & STATE_SYSTEM_INVISIBLE)
		return FALSE;

	// Tool windows should not be displayed either, these do not appear in the
	// task bar.
	if (GetWindowLong(hwnd, GWL_EXSTYLE) & WS_EX_TOOLWINDOW)
		return FALSE;

	return TRUE;
}

Server::~Server()
{
}
