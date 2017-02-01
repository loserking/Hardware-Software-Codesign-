#include "keyboard.h"
SC_MODULE(controller)
{

	sc_out<sc_uint<3> >	row; 
	sc_in<sc_uint<5> >	column;
	sc_in_clk			clk;
	sc_out<sc_uint<13> >  code;
	sc_in<bool>		shift;
	sc_in<bool>		ctrl;
	sc_in<bool>		alt;
	keyboard *kyb;
	sc_signal<int>		numberofkey;
	sc_signal<int>		specialkey;
	int					state;
	int					counter;
	int					reg1;

  SC_CTOR(controller)
  {
	kyb = new keyboard("KYB");
	kyb->row(row); 
    kyb->column(column);
	kyb->shift(shift);
	kyb->ctrl(ctrl);
	kyb->alt(alt);
	kyb->numberofkey(numberofkey);
	kyb->specialkey(specialkey);
	state=0;
	counter=0;
	SC_METHOD(FSM);
	sensitive<<clk;
  };
	void FSM(); 
};