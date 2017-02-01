#include "creg.h"

void	creg::entry()
{
    unsigned char a,d,r,w;
    a=addr.read();
    d=wr_data.read();
    r=re.read();
    w=we.read();
 
    if (r&&(a==baddr)) printf("  --CREGR:A=%02X D=%02X\n",a,rd_data_reg);
    if (w) {
	regs[a]=d;
	printf("  --CREGW:A=%02X D=%02X\n",a,d);
    }
    baddr=a;
}

void	creg::rout()
{
    unsigned a;
    a=addr.read();
    rd_data_reg=regs[a];
    rd_data.write(rd_data_reg);
}
