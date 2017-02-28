#include "Observee.h"

/* 
	Il metodo cerca nel vettore degli observer se l'observer
    passato come parametro è già presente. Se sì, ritorna falso.
    Se no lo aggiunge e ritorna vero.
*/
bool Observee::addObserver(Observer* observer) {
	vector<Observer*>::iterator it = find(observerVector.begin(), observerVector.end(), observer);
	if (it != observerVector.end())
		return false;
	observerVector.push_back(observer);
	return true;
}

/* 
	Il metodo cerca nel vettore degli observer se l'observer
    passato come parametro è presente. Se no, ritorna falso.
    Se sì lo elimina e ritorna vero.
*/
bool Observee::removeObserver(Observer* observer) {
	vector<Observer*>::iterator it = find(observerVector.begin(), observerVector.end(), observer);

	if (it == observerVector.end())
		return false;
	observerVector.erase(remove(observerVector.begin(), observerVector.end(), observer));
	return true;
}

/*
	Il metodo invia una notifica a tutti gli observer
	presenti nel vettore. Ritorna vero se ci sono degli
	observer nel vettore.
*/
bool Observee::notifyObservers() {
	for_each(observerVector.begin(), observerVector.end(), Notifier(this));
	return (observerVector.size() > 0);
}