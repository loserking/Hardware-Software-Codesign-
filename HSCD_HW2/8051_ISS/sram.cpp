#include "sram.h"

void	sram::entry()
{
    unsigned char a,d,r,w,rdy;
    const char *s=name();
    a=addr.read();
    d=wr_data.read();
    r=re.read();
    w=we.read();

  
    if (rd_delay==0) {
	rd_cnt=0;
	rd_ready=true;
//	if (r&&(a==baddr.read())) printf("  --SRAMR(%s):A=%02X D=%02X\n",name(),a,rd_data_reg);
    } else {
	if (r) {
	    if (a!=baddr.read()) rd_cnt=rd_delay;
	    else if (rd_cnt==0) rd_cnt=rd_delay; else rd_cnt--;
	    if (rd_cnt==0) {
		rd_ready=true;
		printf("  --SRAMR(%s):A=%02X D=%02X\n",name(),a,rd_data_reg);
	    } else rd_ready=false;
	} else {
	    if (rd_cnt==0) rd_cnt=rd_delay;
	    else if (rd_cnt!=rd_delay) rd_cnt--;
	    if ((rd_cnt==0)||(rd_cnt==rd_delay)) rd_ready=true; else rd_ready=false;
	}
    }

    if (wr_delay==0) {
	if (w) {
	    mem[a]=d;
	    printf("  --SRAMW(%s):A=%02X D=%02X\n",name(),a,d);
	}
	wr_ready=true;
    } else {
	if (w) {
	    rdy=ready.read();
	    if (wr_cnt==0) wr_cnt=wr_delay; else wr_cnt--;
	    if (wr_cnt==0) {
		    mem[a]=d;
		   printf("  --SRAMW(%s):A=%02X D=%02X\n",name(),a,d);
		    wr_ready=true;
	    } else wr_ready=false;
	} else {
	    if (wr_cnt==0) wr_cnt=wr_delay;
	    else if (wr_cnt!=wr_delay) wr_cnt--;
	    if ((wr_cnt==0)||(wr_cnt==wr_delay)) wr_ready=true; else wr_ready=false;
	}
    }
    bre.write(r);
    bwe.write(w);
    baddr.write(a);
}

void	sram::set_ready()
{
    bool oe;
    unsigned char a;
    const char *s=name();
    if (rd_delay==0) {
	a=addr.read();
	rd_data_reg=mem[a];	    //read to reg whenever addr change
	oe=true; //register always output;
    } else {
	a=baddr.read();
	rd_data_reg=mem[a];	    //read to reg whenever addr change
	oe=re.read();
    }
    if (oe) rd_data.write(rd_data_reg);
    else rd_data.write(0x00);

    
    dready=true;
    if (re.read()) {
	if (addr.read()!=baddr.read()) dready=false;
	if (rd_delay!=0) {
	    if (!bre.read()&&re.read()) dready=false;
	    if (!rd_ready.read()) dready=false;
	}
    }
    if (we.read()) {
	if (!bwe.read()) dready=false;
	if (wr_delay!=0) {
	    if (!wr_ready.read()) dready=false;
	}
    }
    ready.write(dready);
}
