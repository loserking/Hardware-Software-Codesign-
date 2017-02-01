#ifndef	__MC_8051_H
#define __MC_8051_H
#include <systemc.h>
typedef struct  {
unsigned char   port0o   ;
unsigned char   port1o   ;
unsigned char   port2o   ;
unsigned char   port3o   ;
unsigned char   ACC      ;
unsigned char   BCC      ;
unsigned char   IE       ;
unsigned char   IP       ;
unsigned char   SP       ;
unsigned char   DPL      ;
unsigned char   DPH      ;
unsigned char   TCON     ;
unsigned char   TL0      ;
unsigned char   TL1      ;
unsigned char   TH0      ;
unsigned char   TH1      ;
unsigned char   TU0      ;
unsigned char   TU1      ;
unsigned char   RCAPL0   ;
unsigned char   RCAPL1   ;
unsigned char   RCAPH0   ;
unsigned char   RCAPH1   ;
unsigned char   RCAPU0   ;
unsigned char   RCAPU1   ;
bool            pidle    ;
bool            CF       ;
bool            AC       ;
bool            OV       ;
bool		jmp	 ;
unsigned short	newPC	 ;
unsigned char   REG[8]   ;
unsigned char   ram[256] ;
unsigned	PLX_P;
unsigned long	PLX_R[8];
}       VARS_8051;

void    show_debug(const VARS_8051 *news, const VARS_8051 *olds);

SC_MODULE(core_8051)
{
sc_in_clk			clk;
sc_in<sc_uint<1> >		reset;
sc_out<sc_uint<1> > 		poff;
sc_in<sc_uint<8> >		intreq;
sc_out<sc_uint<8> >		port0o,port1o,port2o,port3o;
sc_in<sc_uint<8> >		port0i,port1i,port2i,port3i;
sc_out<sc_uint<8> >		xreg_addr;
sc_out<sc_uint<8> >		xreg_wr_data;
sc_in<sc_uint<8> >		xreg_rd_data;
sc_out<sc_uint<1> >		xreg_re;
sc_out<sc_uint<1> >		xreg_we;
sc_in<sc_uint<1> >		xreg_ready;
sc_out<sc_uint<16> >		xram_addr;
sc_out<sc_uint<8> >		xram_wr_data;
sc_in<sc_uint<8> >		xram_rd_data;
sc_out<sc_uint<1> >		xram_re;
sc_out<sc_uint<1> >		xram_we;
sc_in<sc_uint<1> >		xram_ready;


	sc_signal<sc_uint<16> >	PC;
	sc_signal<sc_uint<8> >	ACC;	//E0
	sc_signal<sc_uint<8> >	BCC;	//F0
	sc_signal<sc_uint<8> >	IE;	//A8
	sc_signal<sc_uint<8> >	IP;	//B8
	sc_signal<sc_uint<8> >	SP;	//81
	sc_signal<sc_uint<8> >	DPL;	//82
	sc_signal<sc_uint<8> >	DPH;	//83
	sc_signal<sc_uint<8> >	TCON;	//88
	sc_signal<sc_uint<8> >	TL0;	//8A
	sc_signal<sc_uint<8> >	TL1;	//8B
	sc_signal<sc_uint<8> >	TH0;	//8C
	sc_signal<sc_uint<8> >	TH1;	//8D
	sc_signal<sc_uint<8> >	TU0;	//8E
	sc_signal<sc_uint<8> >	TU1;	//8F
	sc_signal<sc_uint<8> >	RCAPL0;	//9A
	sc_signal<sc_uint<8> >	RCAPL1;	//9B
	sc_signal<sc_uint<8> >	RCAPH0;	//9C
	sc_signal<sc_uint<8> >	RCAPH1;	//9D
	sc_signal<sc_uint<8> >	RCAPU0;	//9E
	sc_signal<sc_uint<8> >	RCAPU1;	//9F
	sc_signal<sc_uint<1> >	CF;
	sc_signal<sc_uint<1> >	AC;
	sc_signal<sc_uint<1> >	OV;
	sc_signal<sc_uint<1> >	pidle;
	sc_signal<sc_uint<1> >	xreg_addr_sel;
	sc_signal<sc_uint<1> >	xreg_waitready;
	sc_signal<sc_uint<1> >	xram_waitready;
	sc_signal<sc_uint<8> >	REG[8];
	sc_signal<sc_uint<2> >	bextint;
	sc_signal<sc_uint<8> >	intack;
	sc_signal<sc_uint<16> >	iret_high;
	sc_signal<sc_uint<16> >	iret_low;
	unsigned char	rom_addr_high;
	bool		jmp;
	unsigned char   rom[65536*4];
	unsigned char   ram[256];

	bool		loadrom(const char *filename);
	void		show_assembly();

	unsigned char	PSW();
	unsigned char	read_SFR(unsigned char maddr,bool rmw);
	unsigned char	read_direct(unsigned char maddr,bool rmw);
	unsigned char	read_indirect(unsigned char maddr);
	unsigned char	read_bit(unsigned char maddr,unsigned char bit_addr,bool rmw);
	void		write_SFR(unsigned char maddr,unsigned char v);
	void		write_direct(unsigned char maddr,unsigned char v);
	void		write_indirect(unsigned char maddr,unsigned char v);
	void		write_bit(unsigned char maddr,unsigned char bit_addr,unsigned char v);
	unsigned char	add(unsigned char a,unsigned char b,unsigned char c);
	unsigned char	sub(unsigned char a,unsigned char b,unsigned char c);
	unsigned char	da(unsigned char a,unsigned char cf,unsigned char ac);
	bool		is_waiting();
	void		set_xram();
	void		set_xreg();
	unsigned char	check_interrupt();
	unsigned char	set_interrupt(bool skip);
	void		extintr(unsigned char &tcon);
	void		timer(unsigned char &tcon);
	void		entry();
	void		save_vars(VARS_8051 *dst);
	SC_CTOR(core_8051)
    	{
		SC_METHOD(entry);
		sensitive << clk.pos();
		SC_METHOD(set_xram);
		sensitive<<ACC<<DPL<<DPH;
		SC_METHOD(set_xreg);
		sensitive<<ACC<<REG[0]<<REG[1]<<xreg_addr_sel;
	}
};

#endif
