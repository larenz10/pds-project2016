#pragma once
#include "Applicazione.h"
#include <iostream>
#include <vector>

using namespace std;

/* 
	È il server che osserva i comportamenti 
	dell'applicazione, eredita dalla classe
	Observer e invia al client (qui stampa)
	le notifiche.
*/
class Server {
private:
	vector<Applicazione*> apps;

public:
	Server() {}
	~Server() {}
	void aggiungiApp(Applicazione* app);
	void rimuoviApp(Applicazione* app);
	void notifica(Applicazione* app, string action);
	void togliFocus(Applicazione* app);
};