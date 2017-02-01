#include "systemc.h"
#include "keyboard.h"
void keyboard::lookup(){

	if (row.read()==1 && column.read()==1)
		numberofkey=110;
	else
		numberofkey=0;
}
