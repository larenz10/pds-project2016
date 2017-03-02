#include "Server.h"
#include "Applicazione.h"

int main(int argc, char* argv[]) 
{
	Server* s = new Server();
	Applicazione* a1 = new Applicazione(s, "Programma1");
	Applicazione* a2 = new Applicazione(s, "Programma2");
	Applicazione* a3 = new Applicazione(s, "Programma3");
	a1->setFocus(false);
	a2->setFocus(true);
	a3->setFocus(false);
	delete a1;
	delete a3;
	delete a2;
}
