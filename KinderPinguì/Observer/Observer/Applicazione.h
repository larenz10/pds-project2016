#pragma once
#include <iostream>
#include <memory>
#include <Windows.h>
#include "Server.h"

using namespace std;

/*
	L'Applicazione viene osservata dal server.
	Ogni volta che succede qualcosa all'Applicazione,
	il Server se ne deve accorgere e notificare
	il Client. 
	Valutare se fare la classe server come shared_ptr
*/
class Applicazione {
private:
	Server* server;
	string nome;
	//LPTSTR* nome;
	//DWORD processo;
	//HICON icona;
	bool focus;

public:
	//Applicazione(Server* s, LPTSTR* n, DWORD p, HICON i);
	Applicazione(Server* s, string n);
	~Applicazione();
	//LPTSTR* getNome();
	string getNome();
	void setFocus(bool f);
};