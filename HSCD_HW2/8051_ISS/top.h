#ifndef	__CORE_8051_H
#define __CORE_8051_H
#include <systemc.h>
#include "core_8051.h"
#include "sram.h"
#include "Xram.h"
#include "FIR_asic.h"

SC_MODULE(top)
{
sc_in_clk		clk;
sc_in_clk       clk_800;
sc_in<sc_uint<1> >	reset;
sc_out<sc_uint<1> >	poff;


        sc_signal<sc_uint<8> >		intreq;
        sc_signal<sc_uint<8> >		port0o,port1o,port2o,port3o;
        sc_signal<sc_uint<8> >		port0i,port1i,port2i,port3i;
        sc_signal<sc_uint<8> >		xreg_addr;
        sc_signal<sc_uint<8> >		xreg_wr_data;
        sc_signal<sc_uint<8> >		xreg_rd_data;
        sc_signal<sc_uint<1> >		xreg_re;
        sc_signal<sc_uint<1> >		xreg_we;
        sc_signal<sc_uint<1> >		xreg_ready;
        sc_signal<sc_uint<16> >		xram_addr;
        sc_signal<sc_uint<8> >		xram_wr_data;
        sc_signal<sc_uint<8> >		xram_rd_data;
        sc_signal<sc_uint<1> >		xram_re;
        sc_signal<sc_uint<1> >		xram_we;
        sc_signal<sc_uint<1> >		xram_ready;


		sc_signal<sc_uint<8> >		xram_addr8;
        sc_signal<sc_uint<8> >		xrambig_rd_data;
        sc_signal<sc_uint<1> >		xrambig_re;
        sc_signal<sc_uint<1> >		xrambig_we;
		sc_signal<sc_uint<1> >		xrambig_ready;


	FIR_asic	*asic1;
	core_8051	*core;
	sram		*xreg;
	xram        *xrambig;
	void		mux_xram();
	void		entry();

	SC_CTOR(top)
	{	    
	
		asic1=new		FIR_asic("FIR_asic");
		asic1->clk		(clk);
		asic1->A			(port0o);
		asic1->B			(port3o);
		asic1->C			(port3i);
		asic1->D			(port1i);
	
	    core=new core_8051("core");
	    core->clk	    ( clk_800	);
	    core->reset	    ( reset	);
	    core->poff	    ( poff	);
	    core->intreq    ( intreq	);
	    core->port0o ( port0o        );
	    core->port1o ( port1o        );
	    core->port2o ( port2o        );
	    core->port3o ( port3o        );
	    core->port0i ( port0i        );
	    core->port1i ( port1i        );
	    core->port2i ( port2i        );
	    core->port3i ( port3i        );
	    core->xreg_addr      ( xreg_addr     );
	    core->xreg_wr_data   ( xreg_wr_data  );
	    core->xreg_rd_data   ( xreg_rd_data  );
	    core->xreg_re        ( xreg_re       );
	    core->xreg_we        ( xreg_we       );
	    core->xreg_ready     ( xreg_ready    );
	    core->xram_addr      ( xram_addr     );
	    core->xram_wr_data   ( xram_wr_data  );
	    core->xram_rd_data   ( xram_rd_data  );
	    core->xram_re        ( xram_re       );
	    core->xram_we        ( xram_we       );
	    core->xram_ready     ( xram_ready    );

	    xreg=new sram("xreg");
	    xreg->clk		( clk		);
	    xreg->addr		( xreg_addr     );
	    xreg->wr_data	( xreg_wr_data  );
	    xreg->rd_data	( xreg_rd_data  );
	    xreg->re		( xreg_re       );
	    xreg->we		( xreg_we       );
	    xreg->ready		( xreg_ready    );
	    xreg->rd_delay=0;
	    xreg->wr_delay=0;
	    xreg->rd_cnt=xreg->rd_delay;
	    xreg->wr_cnt=xreg->wr_delay;
	    
	    xrambig=new xram("Xram");
	    xrambig->clk		( clk		);
	    xrambig->addr	( xram_addr    );
	    xrambig->wr_data	( xram_wr_data  );
	    xrambig->rd_data	( xrambig_rd_data);
	    xrambig->re		( xrambig_re     );
	    xrambig->we		( xrambig_we     );
	    xrambig->ready	( xrambig_ready  );
	    xrambig->rd_delay=2;
	    xrambig->wr_delay=2;
	    xrambig->rd_cnt=0;
	    xrambig->wr_cnt=0;

	    SC_METHOD(entry);
	    sensitive << clk.pos();

	    SC_METHOD(mux_xram);
	    sensitive<<xram_addr<<xram_re<<xram_we
			<<xrambig_rd_data<<xrambig_ready<<clk;
	}
};
#endif
