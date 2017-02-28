#pragma once
#include <vector>

using namespace std;

class Observee;

/* Classe dell'Observer */
class Observer {
private:
	/* Disabilito costruttore di copia e operatore di assegnazione */
	Observer(const Observer& ref);
	Observer& operator=(const Observer& ref);

protected:
	Observer() {}

public:
	virtual ~Observer() {}	//distruttore
	virtual void notify (Observee* observee) {}	//metodo di notifica da implementare nelle sottoclassi
};