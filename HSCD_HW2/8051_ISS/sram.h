#ifndef	__SRAM_H
#define __SRAM_H
#include <systemc.h>
SC_MODULE(sram)
{
sc_in_clk		clk;
sc_in<sc_uint<8> >	addr;
sc_in<sc_uint<8> >	wr_data;
sc_in<sc_uint<1> >	re;
sc_in<sc_uint<1> >	we;
sc_out<sc_uint<8> >	rd_data;
sc_out<sc_uint<1> >	ready;

	sc_signal<sc_uint<8> >	baddr;
	sc_signal<sc_uint<1> >	rd_ready, wr_ready;
	sc_signal<sc_uint<1> >	bre,bwe;
	unsigned char	rd_data_reg;
	unsigned char	rd_delay, wr_delay; //need setup at initial
	unsigned char	rd_cnt, wr_cnt;
	unsigned char	mem[256];
    bool dready;
	void	entry();
	void	rout();
	void	set_ready();
	SC_CTOR(sram)
    	{
		SC_METHOD(entry);
		sensitive << clk.pos();
		SC_METHOD(set_ready);
		//sensitive<<baddr<<addr<<rd_ready<<wr_ready<<re<<bre<<we<<bwe<<clk;
		sensitive << clk.neg();
	}
};
#endif
