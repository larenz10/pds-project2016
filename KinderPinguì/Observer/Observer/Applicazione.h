#pragma once
#include <Windows.h>
#include "Observee.h"

/*
	L'Applicazione viene osservata dal server.
	Ogni volta che succede qualcosa all'Applicazione,
	questa il Server se ne deve accorgere e notificare
	il Client.
*/
class Applicazione : public Observee {
private:
	LPTSTR* nome;
	DWORD processo;
	HICON icona;
	bool focus;

public:
	//Costruttore
	Applicazione(LPTSTR* n, DWORD p, HICON i) {
		nome = n;
		processo = p;
		icona = i;
		focus = true;
	}
	//Distruttore
	virtual ~Applicazione() {
		delete nome;
		nome = nullptr;
	}
};