#ifndef	__SRAM__H
#define __SRAM_H
#include <systemc.h>
SC_MODULE(creg)
{
sc_in_clk		clk;
sc_in<sc_uint<8> >	addr;
sc_in<sc_uint<8> >	wr_data;
sc_in<sc_uint<1> >	re;
sc_in<sc_uint<1> >	we;
sc_out<sc_uint<8> >	rd_data;

	unsigned char	baddr;
	unsigned char	rd_data_reg;
	unsigned char	regs[256];
	bool dready;
	void	entry();
	void	rout();
	SC_CTOR(creg)
    	{
		SC_METHOD(entry);
		sensitive << clk.pos();
		SC_METHOD(rout);
		sensitive << clk.neg();
	}
};
#endif
