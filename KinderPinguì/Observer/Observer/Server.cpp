#include "Server.h"
#include <algorithm>

void Server::aggiungiApp(Applicazione * app)
{
	apps.push_back(app);
	notifica(app, "new");
	
}

void Server::rimuoviApp(Applicazione * app)
{
	apps.erase(remove(apps.begin(), apps.end(), app), apps.end());
	notifica(app, "delete");
}

void Server::notifica(Applicazione * app, string action)
{
	string n = app->getNome();
	if (action == "new") {
		cout << "Notifica: nuova applicazione " << n << endl;
	}
	else if (action == "delete") {
		cout << "Notifica: chiusa applicazione " << n << endl;
	}
	else if (action == "focus") {
		cout << "Notifica: l'applicazione " << n << " ha ora il focus." << endl;
	}
}
/* 
	La funzione toglie il focus a tutte le applicazioni
	ad eccezione di quella che l'ha appena guadagnato.
*/
void Server::togliFocus(Applicazione * app)
{
	for (auto it : apps) {
		if (it != app)
			it->setFocus(false);
	}
}
