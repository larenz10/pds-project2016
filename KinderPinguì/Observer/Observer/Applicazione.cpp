#include "Applicazione.h"
/* Commentato per ragioni di test
Applicazione::Applicazione(Server* s, LPTSTR* n, DWORD p, HICON i)
{
	server = s;
	nome = n;
	processo = p;
	icona = i;
	focus = true;				//All'avvio di un'applicazione supponiamo che abbia sempre il focus
	server->aggiungiApp(this);
}

*/

Applicazione::Applicazione(Server * s, string n)
{
	server = s;
	nome = n;
}

Applicazione::~Applicazione()
{
	server->rimuoviApp(this);
	delete server;
	server = nullptr;
	//delete nome; 
	//nome = nullptr;
}

string Applicazione::getNome() {
	return nome;
}

/*
LPTSTR * Applicazione::getNome()
{
	return nome;
}
*/
void Applicazione::setFocus(bool f) {
	focus = f;
	if (focus == true) {
		server->togliFocus(this);
		server->notifica(this, "focus");
	}
}

