#pragma once
#include "Observer.h"
#include <iostream>

/* 
	È il server che osserva i comportamenti 
	dell'applicazione, eredita dalla classe
	Observer e invia al client (qui stampa)
	le notifiche.
*/
class Server : public Observer {
private:

public:
	Server();
	virtual ~Server();

	void Print() const;
	virtual void notify(Observee* observee);
};