#include "periph.h"
#include "systemc.h"

void periph::FSM()
{
	int k;
	if((k=P1i.read()) == 15)
		k=k;
	k=k;
}