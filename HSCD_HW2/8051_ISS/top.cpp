#include "top.h"

void top::entry()
{
    if (reset.read()) {
	//port0i.write(0xFF);
	//port1i.write(0xFF);
	//port2i.write(0xFF);
	//port3i.write(0xFF);
    }
}

void top::mux_xram()
{
    unsigned short a=xram_addr.read();
	bool    a03=TRUE;
	xrambig_re=a03&&xram_re.read();
	xrambig_we=a03&&xram_we.read();
    xram_rd_data=(a03)?xrambig_rd_data.read()
				:0x00;
    bool rdy=(a03)?xrambig_ready.read()
			:true;
    xram_ready=rdy;
    xram_addr8=xram_addr.read()&0xFF;
//	if(xram_we.read())
//		printf("xram we %d%d%d%d\n",a01,a02,a03,a04);
/*bool a1=xram01_ready.read();
bool b1=xram01->dready;
bool c1=xram01->ready.read();
bool a2=xram02_ready.read();
bool b2=xram02->dready;
bool c2=xram02->ready.read();*/
    //printf("%d %d %d %d %d %d\n",a1,b1,c1,a2,b2,c2);
}
