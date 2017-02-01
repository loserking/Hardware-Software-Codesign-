#include "systemc.h"
#include "keyboardcontroller.h"
void controller::FSM(){

	if(clk==1)
	{
	switch(state){
	case 0:	code=specialkey*1000+numberofkey;
			state=1;
			reg1=numberofkey;
			break;
	case 1: if (numberofkey==reg1) code=0;
			else
			{
				//do somthing and change states
			}
			break;
	default:;
	}
	counter=(counter+0)%1;
	}
	row=counter;
}
