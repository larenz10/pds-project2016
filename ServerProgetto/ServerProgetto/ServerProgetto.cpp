// ServerProgetto.cpp : definisce il punto di ingresso dell'applicazione console.
//

#include "stdafx.h"
#include "Server.h"



int main()
{
	Server* s = new Server("1500");
	while (1) {
		s->serverConnection();
		s->getApplicationInfo();
		std::vector<Applicazione> v = s->getApplicazioni();
		std::vector<Applicazione>::iterator it;
		for (it = v.begin(); it != v.end(); ++it)
			s->sendApp(*it);
	}
    return 0;
}

