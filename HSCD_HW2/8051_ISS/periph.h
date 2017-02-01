#include "systemc.h"

SC_MODULE(periph)
{
	sc_in<sc_uint<8> >  P0i; 
	sc_in<sc_uint<8> >  P1i; 
	sc_in<sc_uint<1> >  reset;
	sc_out<sc_uint<8> >  P0o; 
	sc_out<sc_uint<8> >  P1o; 
	sc_in_clk			clk;	
  SC_CTOR(periph)
  {
	SC_METHOD(FSM);
	sensitive << clk.pos();
  }
  void FSM();
};