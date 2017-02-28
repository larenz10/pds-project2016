#pragma once

#include "Observer.h"
#include <vector>
#include <algorithm>

class Observer;

/* Classe dell'Observee */
class Observee {
private:
	Observer* observer;					//Un solo observer
	vector<Observer*> observerVector;	//Per più Observer
	/* Disabilito costruttore di copia e operatore di assegnazione */
	Observee(const Observee& ref);
	Observee& operator=(const Observee& ref);

protected:
	Observee() {}

public:
	virtual ~Observee() {}		//distruttore

	Observer* getObserver() const {
		return observer;
	}

	void setObserver(Observer* obs) {
		observer = obs;
	}

	/* In caso di più observer */
	bool addObserver(Observer* observer);
	bool removeObserver(Observer* observer);
	bool notifyObservers();
};