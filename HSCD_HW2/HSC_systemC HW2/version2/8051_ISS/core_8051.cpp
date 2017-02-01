#include <stdio.h>
#include "core_8051.h"
#include "op_bytes.cpp"

char fgets0(char *s,int n,FILE *F)
{
    s[0]=0;
    fgets(s,n,F);
    return(s[0]);
}

bool core_8051::loadrom(const char *filename)
{
    FILE *FR;
    char s[100];
    unsigned int u;
    int i;
    FR=fopen(filename,"r");
    if (FR==NULL) return(FALSE);
    i=0;
    while(fgets0(s,100,FR)!=0) {
        sscanf(s,"%x",&u);
        rom[i]=u;
        i++;
    }
    fclose(FR);
    for(i=0;i<256;i++) ram[i]=0;
    return(true);
}

unsigned char	core_8051::PSW()
{
    unsigned char bCF,bAC,bOV,bP,c;
    int i;
    bCF=CF.read()&0x01;
    bAC=AC.read()&0x01;
    bOV=OV.read()&0x01;
    c=ACC.read();
    bP=0;
    for(i=0;i<8;i++) {
        if (c&0x01) bP^=0x01;
        c>>=1;
    }
    c=(bCF<<7)|(bAC<<6)|(bOV<<2)|bP;
    return(c);
}

unsigned char core_8051::read_SFR(unsigned char maddr,bool rmw)
{
    if (maddr==0x80) {if (rmw) return(port0o.read()); else return(port0i.read());}
    else if (maddr==0x90) {if (rmw) return(port1o.read()); else return(port1i.read());}
    else if (maddr==0xA0) {if (rmw) return(port2o.read()); else return(port2i.read());}
    else if (maddr==0xB0) {if (rmw) return(port3o.read()); else return(port3i.read());}
    else if (maddr==0xD0) return(PSW());
    else if (maddr==0xE0) return(ACC.read());
    else if (maddr==0xF0) return(BCC.read());
    else if (maddr==0x81) return(SP.read());
    else if (maddr==0xA8) return(IE.read());
    else if (maddr==0xB8) return(IP.read());
    else if (maddr==0x82) return(DPL.read());
    else if (maddr==0x83) return(DPH.read());
    else if (maddr==0x88) return(TCON.read());
    else if (maddr==0x8A) return(TL0.read());
    else if (maddr==0x8B) return(TL1.read());
    else if (maddr==0x8C) return(TH0.read());
    else if (maddr==0x8D) return(TH1.read());   
    else if (maddr==0x8E) return(TU0.read());
    else if (maddr==0x8E) return(TU1.read());   
    else if (maddr==0x9A) return(RCAPL0.read());
    else if (maddr==0x9B) return(RCAPL1.read());
    else if (maddr==0x9C) return(RCAPH0.read());
    else if (maddr==0x9D) return(RCAPH1.read());   
    else if (maddr==0x9E) return(RCAPU0.read());
    else if (maddr==0x9E) return(RCAPU1.read());   
//    else if (maddr==0xF8) return(plx.P);   
    else return(0);
}

unsigned char core_8051::read_direct(unsigned char maddr,bool rmw)
{
    if (maddr&0x80) return(read_SFR(maddr,rmw));
    else return(ram[maddr]);
}

unsigned char	core_8051::read_indirect(unsigned char maddr)
{
    return(ram[maddr]);
}

unsigned char	core_8051::read_bit(unsigned char maddr,unsigned char bit_addr,bool rmw)
{
    unsigned char v;
    v=read_direct(maddr,rmw);
    v=(v>>bit_addr)&0x01;
    return(v);
}

void		core_8051::write_SFR(unsigned char maddr,unsigned char v)
{
    if (maddr==0x80) 	port0o.write(v);
    else if (maddr==0x90) port1o.write(v);
    else if (maddr==0xA0) port2o.write(v);
    else if (maddr==0xB0) port3o.write(v);
    else if (maddr==0xD0) {
	CF.write(v>>7);
	AC.write((v>>6)&0x01);
	OV.write((v>>2)&0x01);
    }
    else if (maddr==0xE0) ACC.write(v);
    else if (maddr==0xF0) BCC.write(v);
    else if (maddr==0x81) SP.write(v);
    else if (maddr==0x87) {
	poff.write((v>>1)&0x01);
	pidle.write(v&0x01);
    }
    else if (maddr==0xA8) IE.write(v);
    else if (maddr==0xB8) IP.write(v);
    else if (maddr==0x82) DPL.write(v);
    else if (maddr==0x83) DPH.write(v);
    else if (maddr==0x88) TCON.write(v);
    else if (maddr==0x8A) TL0.write(v);
    else if (maddr==0x8B) TL1.write(v);
    else if (maddr==0x8C) TH0.write(v);
    else if (maddr==0x8D) TH1.write(v);   
    else if (maddr==0x8E) TU0.write(v);
    else if (maddr==0x8F) TU1.write(v);   
    else if (maddr==0x9A) RCAPL0.write(v);
    else if (maddr==0x9B) RCAPL1.write(v);
    else if (maddr==0x9C) RCAPH0.write(v);
    else if (maddr==0x9D) RCAPH1.write(v);   
    else if (maddr==0x9E) RCAPU0.write(v);
    else if (maddr==0x9F) RCAPU1.write(v);   
}

void		core_8051::write_direct(unsigned char maddr,unsigned char v)
{
    if (maddr&0x80) write_SFR(maddr,v);
    else ram[maddr]=v;
}

void		core_8051::write_indirect(unsigned char maddr,unsigned char v)
{
    ram[maddr]=v;
}

void		core_8051::write_bit(unsigned char maddr,unsigned char bit_addr,unsigned char v)
{
    unsigned char w;
    unsigned char bit;
    w=read_direct(maddr,true);
    bit=(0x01<<bit_addr);
    v=(v&0x01)<<bit_addr;
    w=(w&(~bit))|v;
    write_direct(maddr,w);
}


unsigned char	core_8051::add(unsigned char a,unsigned char b,unsigned char c)
{
    unsigned char ad1,ad2,ad3,bac,bcf,bov;
    ad1=(a&0x0F)+(b&0x0F)+(c&0x01);
    bac=(ad1>>4);
    ad2=((a&0x70)>>4)+((b&0x70)>>4)+bac;
    bov=(ad2>>3);
    ad3=(a>>7)+(b>>7)+bov;
    bcf=(ad3>>1);
    AC.write(bac);
    CF.write(bcf);
    OV.write(bcf^bov);
    return(((ad3&0x01)<<7)|((ad2&0x07)<<4)|(ad1&0x0F));
}

unsigned char	core_8051::sub(unsigned char a,unsigned char b,unsigned char c)
{
    unsigned char su1,su2,su3,bac,bcf,bov;
    su1=((a&0x0F)|0x10)-(b&0x0F)-(c&0x01);
    bac=((~su1)>>4)&0x01;
    su2=(((a&0x70)>>4)|0x08)-((b&0x70)>>4)-bac;
    bov=((~su2)>>3)&0x01;
    su3=((a>>7)|0x02)-(b>>7)-bov;
    bcf=((~su3)>>1)&0x01;
    AC.write(bac);
    CF.write(bcf);
    OV.write(bcf^bov);
    return(((su3&0x01)<<7)|((su2&0x07)<<4)|(su1&0x0F));
}

unsigned char	core_8051::da(unsigned char a,unsigned char cf,unsigned char ac)
{
    unsigned char da1,da2,bac,bcf;
    da1=(a&0x0F);
    da1=(ac||(da1>9)) ? (da1+6) :da1;
    bac=(da1>>4);
    da2=(((cf&0x01)<<4)|(a>>4))+bac;
    bcf=(da2>>4);
    da2=(da2>9) ? da2+6 : da2;
    CF.write(da2>>4);
    return(((da2<<4)&0xF0)|(da1&0x0F));
}

bool	core_8051::is_waiting()
{
    if (xreg_waitready.read()) return(!xreg_ready.read());
    else if (xram_waitready.read()) return(!xram_ready.read());
    else return(false);
}

void	core_8051::set_xram()
{
    unsigned short xa;
    xa=DPL.read()+((unsigned long)DPH.read()<<8);
    xram_addr.write(xa);
    xram_wr_data.write(ACC.read());
}

void	core_8051::set_xreg()
{
    if (xreg_addr_sel.read()!=0) xreg_addr.write(REG[1].read());
    else xreg_addr.write(REG[0].read());
    xreg_wr_data.write(ACC.read());
}

unsigned char core_8051::check_interrupt()
{
    unsigned char   intw, intv, tcon, ie, ip;
    tcon=TCON.read();

    intv=0;
    ie=IE.read();
    ip=IP.read();
    if (ie&0x80) {
	intw=intack.read();
	if ((intw&ip)==0) {//no high priority interrupt working
	    if ((ip&0x01)&&(ie&0x01)&&(tcon&0x02)) intv=1;
	    else if ((ip&0x02)&&(ie&0x02)&&(tcon&0x20)) intv=2;
	    else if ((ip&0x04)&&(ie&0x04)&&(tcon&0x08)) intv=3;
	    else if ((ip&0x08)&&(ie&0x08)&&(tcon&0x80)) intv=4;
	    if ((intw==0)&&(intv==0)) {//no any interrupt
		if (!(ip&0x01)&&(ie&0x01)&&(tcon&0x02)) intv=1;
		else if (!(ip&0x02)&&(ie&0x02)&&(tcon&0x20)) intv=2;
		else if (!(ip&0x04)&&(ie&0x04)&&(tcon&0x08)) intv=3;
		else if (!(ip&0x08)&&(ie&0x08)&&(tcon&0x80)) intv=4;
	    }
	}
    }
    return(intv);
}

unsigned char	core_8051::set_interrupt(bool skip)
{
    unsigned char   intv, tcon;
    tcon=TCON.read();
    intv=check_interrupt();
    if (!skip&&(intv!=0)) {
	switch(intv) {
	    case 1: tcon&=~0x02; break;
	    case 2: tcon&=~0x20; break;
	    case 3: tcon&=~0x08; break;
	    case 4: tcon&=~0x80; break;
	}
    } else intv=0;

    extintr(tcon);
    timer(tcon);
    TCON.write(tcon);
    return(intv);
}

void	core_8051::extintr(unsigned char &tcon)
{
    unsigned char intr, bintr, p3, ie;
    p3=port3i.read();
    if ((tcon&0x01)!=0) p3^=0x04;
    if ((tcon&0x04)!=0) p3^=0x08;
    intr=0;
    if ((p3&0x04)!=0) intr|=1;
    if ((p3&0x08)!=0) intr|=2;
    bintr=bextint.read();
    ie=IE.read();
    if (ie&0x80) {
	if ((ie&0x01)&&(intr&0x01)&&!(bintr&0x01)) tcon|=0x02;
	if ((ie&0x04)&&(intr&0x02)&&!(bintr&0x02)) tcon|=0x08;
    }
    bextint.write(intr);
}

void	core_8051::timer(unsigned char &tcon)
{
    unsigned long t;
    if (tcon&0x10) {
	t=((unsigned long)TU0.read()<<16)|((unsigned long)TH0.read()<<8)|((unsigned long)TL0.read());
	if (t==0) {
	    tcon|=0x20;
	    t=((unsigned long)RCAPU0.read()<<16)|((unsigned long)RCAPH0.read()<<8)|((unsigned long)RCAPL0.read());
	} else t--;
	TL0.write(t&0xFF);
	TH0.write((t>>8)&0xFF);
	TU0.write((t>>16)&0xFF);
    }
    if (tcon&0x40) {
	t=((unsigned long)TU1.read()<<16)|((unsigned long)TH1.read()<<8)|((unsigned long)TL1.read());
	if (t==0) {
	    tcon|=0x80;
	    t=((unsigned long)RCAPU1.read()<<16)|((unsigned long)RCAPH1.read()<<8)|((unsigned long)RCAPL1.read());
	} else t--;
	TL1.write(t&0xFF);
	TH1.write((t>>8)&0xFF);
	TU1.write((t>>16)&0xFF);
    }
}


void    core_8051::show_assembly()
{
	if (is_waiting()) {
	    printf("\n");
	    return;
	}
	unsigned char intv=check_interrupt();
	if (pidle.read()&&(intv==0)) {
	    printf("\n");
	    return;
	}
        unsigned short ADDR=PC.read();
	unsigned long romaddr=ADDR;
        unsigned char OP0=rom[romaddr];
        unsigned char OP1=rom[romaddr+1];
        unsigned char OP2=rom[romaddr+2];
        unsigned char OP3=rom[romaddr+3];
        unsigned char opcl = (OP0&0x08)?0x08:(((OP0&0x06)==0x06)?0x06:OP0&0x07);
        unsigned char opch = ((OP0&0x0F)==0x01)?(OP0&0x10):(OP0&0xF0);
        unsigned char opc =opch|opcl;
        unsigned short ADDR2=ADDR+2;
        unsigned short ADDRL=OP1*256+OP2;
        unsigned short ADDRA=(ADDR2&0xF800)|(((unsigned short)OP0&0xE0)<<3)|((unsigned short)OP1&0xFF);
        unsigned short ADDRS2=ADDR+2+(char)OP1;
        unsigned short ADDRS3=ADDR+3+(char)OP2;
        unsigned char BIT=OP1&0x07;
        unsigned char BA=(OP1&0x80)?(OP1&0xF8):(((OP1&0x78)>>3)+0x20);
        unsigned char Rn=OP0&0x07;
        unsigned char aRn=OP0&0x01;
	if (intv !=0) {
	    printf("== %04X:         INTERRUPT %d ========\n",ADDR,intv-1);
	    return;
	}
        switch(opc) {
        case 0x00:printf("== %04X:%02X       NOP ========\n",ADDR,OP0); break;
        case 0x01:printf("== %04X:%02X %02X    AJMP  %04Xh ========\n",ADDR,OP0,OP1,ADDRA);  break;
        case 0x02:printf("== %04X:%02X %02X %02X LJMP  %04Xh ========\n",ADDR,OP0,OP1,OP2,ADDRL);  break;
        case 0x03:printf("== %04X:%02X       RR    A ========\n",ADDR,OP0);  break;
        case 0x04:printf("== %04X:%02X       INC   A ========\n",ADDR,OP0);  break;
        case 0x05:printf("== %04X:%02X %02X    INC   %02Xh ========\n",ADDR,OP0,OP1,OP1);  break;
        case 0x06:printf("== %04X:%02X       INC   @R%d ========\n",ADDR,OP0,aRn);  break;
        case 0x08:printf("== %04X:%02X       INC   R%d ========\n",ADDR,OP0,Rn);  break;
        case 0x10:printf("== %04X:%02X %02X %02X JBC   %02Xh.%X,%04Xh ========\n",ADDR,OP0,OP1,OP2,BA,BIT,ADDRS3);  break;
        case 0x11:printf("== %04X:%02X %02X    ACALL %04Xh ========\n",ADDR,OP0,OP1,ADDRA);  break;
        case 0x12:printf("== %04X:%02X %02X %02X LCALL %04Xh ========\n",ADDR,OP0,OP1,OP2,ADDRL);  break;
        case 0x13:printf("== %04X:%02X       RRC   A ========\n",ADDR,OP0);  break;
        case 0x14:printf("== %04X:%02X       DEC   A ========\n",ADDR,OP0);  break;
        case 0x15:printf("== %04X:%02X %02X    DEC   %02Xh ========\n",ADDR,OP0,OP1,OP1);  break;
        case 0x16:printf("== %04X:%02X       DEC   @R%d ========\n",ADDR,OP0,aRn);  break;
        case 0x18:printf("== %04X:%02X       DEC   R%d ========\n",ADDR,OP0,Rn);  break;
        case 0x20:printf("== %04X:%02X %02X %02X JB    %02Xh.%X,%04Xh ========\n",ADDR,OP0,OP1,OP2,BA,BIT,ADDRS3);  break;
        case 0x21:printf("== %04X:%02X %02X    AJMP  %04Xh ========\n",ADDR,OP0,OP1,ADDRA);  break;
        case 0x22:printf("== %04X:%02X       RET ========\n",ADDR,OP0);  break;
        case 0x23:printf("== %04X:%02X       RL    A ========\n",ADDR,OP0);  break;
        case 0x24:printf("== %04X:%02X %02X    ADD   A,#%02Xh ========\n",ADDR,OP0,OP1,OP1);  break;
        case 0x25:printf("== %04X:%02X %02X    ADD   A,%02Xh ========\n",ADDR,OP0,OP1,OP1);  break;
        case 0x26:printf("== %04X:%02X       ADD   A,@R%d ========\n",ADDR,OP0,aRn);  break;
        case 0x28:printf("== %04X:%02X       ADD   A,R%d ========\n",ADDR,OP0,Rn);  break;
        case 0x30:printf("== %04X:%02X %02X %02X JNB   %02Xh.%X,%04Xh ========\n",ADDR,OP0,OP1,OP2,BA,BIT,ADDRS3);  break;
        case 0x31:printf("== %04X:%02X %02X    ACALL %04Xh ========\n",ADDR,OP0,OP1,ADDRA);  break;
        case 0x32:printf("== %04X:%02X       RETI ========\n",ADDR,OP0);  break;
        case 0x33:printf("== %04X:%02X       RLC   A ========\n",ADDR,OP0);  break;
        case 0x34:printf("== %04X:%02X %02X    ADDC  A,#%02Xh ========\n",ADDR,OP0,OP1,OP1);  break;
        case 0x35:printf("== %04X:%02X %02X    ADDC  A,%02Xh ========\n",ADDR,OP0,OP1,OP1);  break;
        case 0x36:printf("== %04X:%02X       ADDC  A,@R%d ========\n",ADDR,OP0,aRn);  break;
        case 0x38:printf("== %04X:%02X       ADDC  A,R%d ========\n",ADDR,OP0,Rn);  break;
        case 0x40:printf("== %04X:%02X %02X    JC    %04Xh ========\n",ADDR,OP0,OP1,ADDRS2);  break;
        case 0x41:printf("== %04X:%02X %02X    AJMP  %04Xh ========\n",ADDR,OP0,OP1,ADDRA);  break;
        case 0x42:printf("== %04X:%02X %02X    ORL   %02Xh,A ========\n",ADDR,OP0,OP1,OP1);  break;
        case 0x43:printf("== %04X:%02X %02X %02X ORL   %02Xh,#%02Xh ========\n",ADDR,OP0,OP1,OP2,OP1,OP2);  break;
        case 0x44:printf("== %04X:%02X %02X    ORL   A,#%02Xh ========\n",ADDR,OP0,OP1,OP1);  break;
        case 0x45:printf("== %04X:%02X %02X    ORL   A,%02Xh ========\n",ADDR,OP0,OP1,OP1);  break;
        case 0x46:printf("== %04X:%02X       ORL   A,@R%d ========\n",ADDR,OP0,aRn);  break;
        case 0x48:printf("== %04X:%02X       ORL   A,R%d ========\n",ADDR,OP0,Rn);  break;
        case 0x50:printf("== %04X:%02X %02X    JNC   %04Xh ========\n",ADDR,OP0,OP1,ADDRS2);  break;
        case 0x51:printf("== %04X:%02X %02X    ACALL %04Xh ========\n",ADDR,OP0,OP1,ADDRA);  break;
        case 0x52:printf("== %04X:%02X %02X    ANL   %02Xh,A ========\n",ADDR,OP0,OP1,OP1);  break;
        case 0x53:printf("== %04X:%02X %02X %02X ANL   %02Xh,#%02Xh ========\n",ADDR,OP0,OP1,OP2,OP1,OP2);  break;
        case 0x54:printf("== %04X:%02X %02X    ANL   A,#%02Xh ========\n",ADDR,OP0,OP1,OP1);  break;
        case 0x55:printf("== %04X:%02X %02X    ANL   A,%02Xh ========\n",ADDR,OP0,OP1,OP1);  break;
        case 0x56:printf("== %04X:%02X       ANL   A,@R%d ========\n",ADDR,OP0,aRn);  break;
        case 0x58:printf("== %04X:%02X       ANL   A,R%d ========\n",ADDR,OP0,Rn);  break;
        case 0x60:printf("== %04X:%02X %02X    JZ    %04Xh ========\n",ADDR,OP0,OP1,ADDRS2);  break;
        case 0x61:printf("== %04X:%02X %02X    AJMP  %04Xh ========\n",ADDR,OP0,OP1,ADDRA);  break;
        case 0x62:printf("== %04X:%02X %02X    XRL   %02Xh,A ========\n",ADDR,OP0,OP1,OP1);  break;
        case 0x63:printf("== %04X:%02X %02X %02X XRL   %02Xh,#%02Xh ========\n",ADDR,OP0,OP1,OP2,OP1,OP2);  break;
        case 0x64:printf("== %04X:%02X %02X    XRL   A,#%02Xh ========\n",ADDR,OP0,OP1,OP1);  break;
        case 0x65:printf("== %04X:%02X %02X    XRL   A,%02Xh ========\n",ADDR,OP0,OP1,OP1);  break;
        case 0x66:printf("== %04X:%02X       XRL   A,@R%d ========\n",ADDR,OP0,aRn);  break;
        case 0x68:printf("== %04X:%02X       XRL   A,R%d ========\n",ADDR,OP0,Rn);  break;
        case 0x70:printf("== %04X:%02X %02X    JNZ   %04Xh ========\n",ADDR,OP0,OP1,ADDRS2);  break;
        case 0x71:printf("== %04X:%02X %02X    ACALL %04Xh ========\n",ADDR,OP0,OP1,ADDRA);  break;
        case 0x72:printf("== %04X:%02X %02X    ORLB  C,%02Xh.%X ========\n",ADDR,OP0,OP1,BA,BIT);  break;
        case 0x73:printf("== %04X:%02X       JMP   @A+DPTR ========\n",ADDR,OP0);  break;
        case 0x74:printf("== %04X:%02X %02X    MOV   A,#%02Xh ========\n",ADDR,OP0,OP1,OP1);  break;
        case 0x75:printf("== %04X:%02X %02X %02X MOV   %02Xh,#%02Xh ========\n",ADDR,OP0,OP1,OP2,OP1,OP2);  break;
        case 0x76:printf("== %04X:%02X %02X    MOV   @R%d,#%02Xh ========\n",ADDR,OP0,OP1,aRn,OP1);  break;
        case 0x78:printf("== %04X:%02X %02X    MOV   R%d,#%02Xh ========\n",ADDR,OP0,OP1,Rn,OP1);  break;
        case 0x80:printf("== %04X:%02X %02X    SJMP  %04Xh ========\n",ADDR,OP0,OP1,ADDRS2);  break;
        case 0x81:printf("== %04X:%02X %02X    AJMP  %04Xh ========\n",ADDR,OP0,OP1,ADDRA);  break;
        case 0x82:printf("== %04X:%02X %02X    ANLB  C,%02Xh.%X ========\n",ADDR,OP0,OP1,BA,BIT);  break;
        case 0x83:printf("== %04X:%02X       MOVC  A,@A+PC ========\n",ADDR,OP0);  break;
        case 0x84:printf("== %04X:%02X       DIV ========\n",ADDR,OP0);  break;
        case 0x85:printf("== %04X:%02X %02X %02X MOV   %02Xh,%02Xh ========\n",ADDR,OP0,OP1,OP2,OP2,OP1);  break;
        case 0x86:printf("== %04X:%02X %02X    MOV   %02Xh,@R%d ========\n",ADDR,OP0,OP1,OP1,aRn);  break;
        case 0x88:printf("== %04X:%02X %02X    MOV   %02Xh,R%d ========\n",ADDR,OP0,OP1,OP1,Rn);  break;
        case 0x90:printf("== %04X:%02X %02X %02X MOV   DPTR,#%02X%02Xh ========\n",ADDR,OP0,OP1,OP2,OP1,OP2);  break;
        case 0x91:printf("== %04X:%02X %02X    ACALL %04Xh ========\n",ADDR,OP0,OP1,ADDRA);  break;
        case 0x92:printf("== %04X:%02X %02X    MOVB  %02Xh.%X,C ========\n",ADDR,OP0,OP1,BA,BIT);  break;
        case 0x93:printf("== %04X:%02X       MOVC  A,@A+DPTR ========\n",ADDR,OP0);  break;
        case 0x94:printf("== %04X:%02X %02X    SUBB  A,#%02Xh ========\n",ADDR,OP0,OP1,OP1);  break;
        case 0x95:printf("== %04X:%02X %02X    SUBB  A,%02Xh ========\n",ADDR,OP0,OP1,OP1);  break;
        case 0x96:printf("== %04X:%02X       SUBB  A,@R%d ========\n",ADDR,OP0,aRn);  break;
        case 0x98:printf("== %04X:%02X       SUBB  A,R%d ========\n",ADDR,OP0,Rn);  break;
        case 0xA0:printf("== %04X:%02X %02X    ORLN  C,/%02Xh.%X ========\n",ADDR,OP0,OP1,BA,BIT);  break;
        case 0xA1:printf("== %04X:%02X %02X    AJMP  %04Xh ========\n",ADDR,OP0,OP1,ADDRA);  break;
        case 0xA2:printf("== %04X:%02X %02X    MOVB  C,%02Xh.%X ========\n",ADDR,OP0,OP1,BA,BIT);  break;
        case 0xA3:printf("== %04X:%02X       INC   DPTR ========\n",ADDR,OP0);  break;
        case 0xA4:printf("== %04X:%02X       MUL ========\n",ADDR,OP0);  break;
//        case 0xA5:printf("== %04X:%02X %02X %02X PLX ",ADDR,OP0,OP1,OP2,OP3);  plx.show_assembly(OP1,OP2,OP3); break;
        case 0xA6:printf("== %04X:%02X %02X    MOV   @R%d,%02Xh ========\n",ADDR,OP0,OP1,aRn,OP1);  break;
        case 0xA8:printf("== %04X:%02X %02X    MOV   R%d,%02Xh ========\n",ADDR,OP0,OP1,Rn,OP1);  break;
        case 0xB0:printf("== %04X:%02X %02X    ANLN  C,/%02Xh.%X ========\n",ADDR,OP0,OP1,BA,BIT);  break;
        case 0xB1:printf("== %04X:%02X %02X    ACALL %04Xh ========\n",ADDR,OP0,OP1,ADDRA);  break;
        case 0xB2:printf("== %04X:%02X %02X    CPLB  %02Xh.%X ========\n",ADDR,OP0,OP1,BA,BIT);  break;
        case 0xB3:printf("== %04X:%02X       CPLB  C ========\n",ADDR,OP0);  break;
        case 0xB4:printf("== %04X:%02X %02X %02X CJNE  A,#%02Xh,%04Xh ========\n",ADDR,OP0,OP1,OP2,OP1,ADDRS3);  break;
        case 0xB5:printf("== %04X:%02X %02X %02X CJNE  A,%02Xh,%04Xh ========\n",ADDR,OP0,OP1,OP2,OP1,ADDRS3);  break;
        case 0xB6:printf("== %04X:%02X %02X %02X CJNE  @R%d,#%02Xh,%04Xh ========\n",ADDR,OP0,OP1,OP2,aRn,OP1,ADDRS3);  break;
        case 0xB8:printf("== %04X:%02X %02X %02X CJNE  R%d,#%02Xh,%04Xh ========\n",ADDR,OP0,OP1,OP2,Rn,OP1,ADDRS3);  break;
        case 0xC0:printf("== %04X:%02X %02X    PUSH  %02Xh ========\n",ADDR,OP0,OP1,OP1);  break;
        case 0xC1:printf("== %04X:%02X %02X    AJMP  %04Xh ========\n",ADDR,OP0,OP1,ADDRA);  break;
        case 0xC2:printf("== %04X:%02X %02X    CLRB  %02Xh.%X ========\n",ADDR,OP0,OP1,BA,BIT);  break;
        case 0xC3:printf("== %04X:%02X       CLRB  C ========\n",ADDR,OP0);  break;
        case 0xC4:printf("== %04X:%02X       SWAP  A ========\n",ADDR,OP0);  break;
        case 0xC5:printf("== %04X:%02X %02X    XCH   A,%02Xh ========\n",ADDR,OP0,OP1,OP1);  break;
        case 0xC6:printf("== %04X:%02X       XCH   A,@R%d ========\n",ADDR,OP0,aRn);  break;
        case 0xC8:printf("== %04X:%02X       XCH   A,R%d ========\n",ADDR,OP0,Rn);  break;
        case 0xD0:printf("== %04X:%02X %02X    POP   %02Xh ========\n",ADDR,OP0,OP1,OP1);  break;
        case 0xD1:printf("== %04X:%02X %02X    ACALL %04Xh ========\n",ADDR,OP0,OP1,ADDRA);  break;
        case 0xD2:printf("== %04X:%02X %02X    SETB  %02Xh.%X ========\n",ADDR,OP0,OP1,BA,BIT);  break;
        case 0xD3:printf("== %04X:%02X       SETB  C ========\n",ADDR,OP0);  break;
        case 0xD4:printf("== %04X:%02X       DA    A ========\n",ADDR,OP0);  break;
        case 0xD5:printf("== %04X:%02X %02X %02X DJNZ  %02Xh,%04Xh ========\n",ADDR,OP0,OP1,OP2,OP1,ADDRS3);  break;
        case 0xD6:printf("== %04X:%02X       XCHD  A,@R%d ========\n",ADDR,OP0,aRn);  break;
        case 0xD8:printf("== %04X:%02X %02X    DJNZ  R%d,%04Xh ========\n",ADDR,OP0,OP1,Rn,ADDRS2);  break;
        case 0xE0:printf("== %04X:%02X       MOVX  A,@DPTR ========\n",ADDR,OP0);  break;
        case 0xE1:printf("== %04X:%02X %02X    AJMP  %04Xh ========\n",ADDR,OP0,OP1,ADDRA);  break;
        case 0xE2:printf("== %04X:%02X       MOVX  A,@R0 ========\n",ADDR,OP0);  break;
        case 0xE3:printf("== %04X:%02X       MOVX  A,@R1 ========\n",ADDR,OP0);  break;
        case 0xE4:printf("== %04X:%02X       CLR   A ========\n",ADDR,OP0);  break;
        case 0xE5:printf("== %04X:%02X %02X    MOV   A,%02Xh ========\n",ADDR,OP0,OP1,OP1);  break;
        case 0xE6:printf("== %04X:%02X       MOV   A,@R%d ========\n",ADDR,OP0,aRn);  break;
        case 0xE8:printf("== %04X:%02X       MOV   A,R%d ========\n",ADDR,OP0,Rn);  break;
        case 0xF0:printf("== %04X:%02X       MOVX  @DPTR,A ========\n",ADDR,OP0);  break;
        case 0xF1:printf("== %04X:%02X %02X    ACALL %04Xh ========\n",ADDR,OP0,OP1,ADDRA);  break;
        case 0xF2:printf("== %04X:%02X       MOVX  @R0,A ========\n",ADDR,OP0);  break;
        case 0xF3:printf("== %04X:%02X       MOVX  @R1,A ========\n",ADDR,OP0);  break;
        case 0xF4:printf("== %04X:%02X       CPL   A ========\n",ADDR,OP0);  break;
        case 0xF5:printf("== %04X:%02X %02X    MOV   %02Xh,A ========\n",ADDR,OP0,OP1,OP1);  break;
        case 0xF6:printf("== %04X:%02X       MOV   @R%d,A ========\n",ADDR,OP0,aRn);  break;
        case 0xF8:printf("== %04X:%02X       MOV   R%d,A ========\n",ADDR,OP0,Rn);  break;
        }
}
void    core_8051::entry()
{
    if (reset.read()) {
	PC.write(0x0000);
	SP.write(0x07);
	DPL.write(0x00);
	DPH.write(0x00);
	ACC.write(0x00);
	BCC.write(0x00);
	CF.write(0x00);
	AC.write(0x00);
	OV.write(0x00);
	IE.write(0x00);
	IP.write(0x00);
	TCON.write(0x05);
	TL0.write(0x00);
	TL1.write(0x00);
	TH0.write(0x00);
	TH1.write(0x00);
	TU0.write(0x00);
	TU1.write(0x00);
	RCAPL0.write(0x00);
	RCAPL1.write(0x00);
	RCAPH0.write(0x00);
	RCAPH1.write(0x00);
	RCAPU0.write(0x00);
	RCAPU1.write(0x00);
	poff.write(0);
	pidle.write(0);
	port0o.write(0xFF);
	port1o.write(0xFF);
	port2o.write(0xFF);
	port3o.write(0xFF);
	intack.write(0x00);
	bextint.write(0x0);
	xreg_addr_sel.write(0);
	xreg_re.write(false);
	xreg_we.write(false);
	xram_re.write(false);
	xram_we.write(false);
	xreg_waitready.write(false);
	xram_waitready.write(false);
//	plx.reset();
	return;
    }
    bool skip=false;
    bool new_xreg_re=false;
    bool new_xreg_we=false;
    bool new_xreg_waitready=false;
    bool new_xram_re=false;
    bool new_xram_we=false;
    bool new_xram_waitready=false;
    unsigned short newPC=PC.read();
    unsigned char vSP=SP.read();
    if (xreg_waitready.read()) {
	if (!xreg_ready.read()) {
	    if (xreg_re.read()!=0) {
		ACC.write(xreg_rd_data.read());
	    }
	    new_xreg_re=xreg_re.read();
	    new_xreg_we=xreg_we.read();
	    new_xreg_waitready=true;
	    skip=true;
	} else {
	    new_xreg_re=false;
	    new_xreg_we=false;
	    new_xreg_waitready=false;
	}
    }
    if (xram_waitready.read()) {
	if (!xram_ready.read()) {
	    if (xram_re.read()!=0) {
		ACC.write(xram_rd_data.read());
	    }
	    new_xram_re=xram_re.read();
	    new_xram_we=xram_we.read();
	    new_xram_waitready=true;
	    skip=true;
	} else {
	    new_xram_re=false;
	    new_xram_we=false;
	    new_xram_waitready=false;
	}
    }
    unsigned char intv=set_interrupt(skip);
    if (intv!=0) {
	unsigned char ip=IP.read();
	unsigned char v=(0x01)<<(intv-1);
	intack.write(intack.read()|v);
	if (ip&v) iret_high.write(newPC);
	else iret_low.write(newPC);
	newPC=(intv-1)*8+0x03;
	jmp=true;
	skip=true;
	if (pidle.read()) pidle.write(0);
    } else {
	if (pidle.read()) skip=true;
    }
    if (!skip) {
        unsigned short ADDR=PC.read();
	unsigned long romaddr=ADDR;
        unsigned char OP0=rom[romaddr];
        unsigned char OP1=rom[romaddr+1];
        unsigned char OP2=rom[romaddr+2];
        unsigned char opcl = (OP0&0x08)?0x08:(((OP0&0x06)==0x06)?0x06:OP0&0x07);
        unsigned char opch = ((OP0&0x0F)==0x01)?(OP0&0x10):(OP0&0xF0);
        unsigned char opc =opch|opcl;
        unsigned short ADDR2=ADDR+2;
        unsigned short ADDRL=OP1*256+OP2;
        unsigned short ADDRA=(ADDR2&0xF800)|(((unsigned short)OP0&0xE0)<<3)|((unsigned short)OP1&0xFF);
        unsigned short ADDRS2=ADDR+2+(char)OP1;
        unsigned short ADDRS3=ADDR+3+(char)OP2;
        unsigned char BIT=OP1&0x07;
        unsigned char BA=(OP1&0x80)?(OP1&0xF8):(((OP1&0x78)>>3)+0x20);
        unsigned char Rn=OP0&0x07;
        unsigned char aRn=OP0&0x01;
	unsigned short dptr=DPH.read()*256+DPL.read();
	unsigned char opca=((opc>>1)&0x78)|((opc&0x08)?0x07:(opc&0x07));
	unsigned char bytes=op_bytes[opca];
	unsigned char vSP=SP.read();
	unsigned char vACC=ACC.read();
	unsigned char vCF=CF.read();
	unsigned char res1,res2;
	unsigned short resw;
	vSP=SP.read();
	newPC=PC.read()+bytes;

	jmp=false;
        switch(opc) {
        case 0x00://printf("== %04X:%02X       NOP ========\n",ADDR,OP0); break;
                break;
        case 0x01://printf("== %04X:%02X %02X    AJMP  %04Xh ========\n",ADDR,OP0,OP1,ADDRA);  break;
		jmp=true;
		newPC=ADDRA;
                break;
        case 0x02://printf("== %04X:%02X %02X %02X LJMP  %04Xh ========\n",ADDR,OP0,OP1,OP2,ADDRL);  break;
		jmp=true;
		newPC=ADDRL;
                break;
        case 0x03://printf("== %04X:%02X       RR    A ========\n",ADDR,OP0);  break;
		res1=(vACC>>1)|(vACC<<7);
		ACC.write(res1);
                break;
        case 0x04://printf("== %04X:%02X       INC   A ========\n",ADDR,OP0);  break;
		res1=vACC+1;
		ACC.write(res1);
                break;
        case 0x05://printf("== %04X:%02X %02X    INC   %02Xh ========\n",ADDR,OP0,OP1,OP1);  break;
		res1=read_direct(OP1,true)+1;
		write_direct(OP1,res1);
                break;
        case 0x06://printf("== %04X:%02X       INC   @R%d ========\n",ADDR,OP0,aRn);  break;
		res1=read_indirect(REG[aRn].read())+1;
		write_indirect(REG[aRn].read(),res1);
                break;
        case 0x08://printf("== %04X:%02X       INC   R%d ========\n",ADDR,OP0,Rn);  break;
		res1=REG[Rn].read()+1;
		REG[Rn].write(res1);
                break;
        case 0x10://printf("== %04X:%02X %02X %02X JBC   %02Xh.%X,%04Xh ========\n",ADDR,OP0,OP1,OP2,BA,BIT,ADDRS3);  break;
		res1=read_bit(BA,BIT,false);
		if (res1) {
		    write_bit(BA,BIT,0);
		    jmp=true;
		    newPC=ADDRS3;
		}
                break;
        case 0x11://printf("== %04X:%02X %02X    ACALL %04Xh ========\n",ADDR,OP0,OP1,ADDRA);  break;
		ram[vSP+1]=newPC&0xFF;
		ram[vSP+2]=newPC>>8;
		SP.write(vSP+2);
		jmp=true;
		newPC=ADDRA;
                break;
        case 0x12://printf("== %04X:%02X %02X %02X LCALL %04Xh ========\n",ADDR,OP0,OP1,OP2,ADDRL);  break;
		ram[vSP+1]=newPC&0xFF;
		ram[vSP+2]=newPC>>8;
		SP.write(vSP+2);
		jmp=true;
		newPC=ADDRL;
                break;
        case 0x13://printf("== %04X:%02X       RRC   A ========\n",ADDR,OP0);  break;
		res1=(vACC>>1)|(vCF<<7);
		CF.write(vACC&1);
		ACC.write(res1);
                break;
        case 0x14://printf("== %04X:%02X       DEC   A ========\n",ADDR,OP0);  break;
		res1=vACC-1;
		ACC.write(res1);
                break;
        case 0x15://printf("== %04X:%02X %02X    DEC   %02Xh ========\n",ADDR,OP0,OP1,OP1);  break;
		res1=read_direct(OP1,true)-1;
		write_direct(OP1,res1);
                break;
        case 0x16://printf("== %04X:%02X       DEC   @R%d ========\n",ADDR,OP0,aRn);  break;
		res1=read_indirect(REG[aRn].read())-1;
		write_indirect(REG[aRn].read(),res1);
                break;
        case 0x18://printf("== %04X:%02X       DEC   R%d ========\n",ADDR,OP0,Rn);  break;
		res1=REG[Rn].read()-1;
		REG[Rn].write(res1);
                break;
        case 0x20://printf("== %04X:%02X %02X %02X JB    %02Xh.%X,%04Xh ========\n",ADDR,OP0,OP1,OP2,BA,BIT,ADDRS3);  break;
		res1=read_bit(BA,BIT,false);
		if (res1) {
		    jmp=true;
		    newPC=ADDRS3;
		}
                break;
        //case 0x21://printf("== %04X:%02X %02X    AJMP  %04Xh ========\n",ADDR,OP0,OP1,ADDRA);  break;
        //        break;
        case 0x22://printf("== %04X:%02X       RET ========\n",ADDR,OP0);  break;
		res1=ram[vSP];
		res2=ram[vSP-1];
		SP.write(vSP-2);
		jmp=true;
		newPC=((unsigned short)res1<<8)|res2;
                break;
        case 0x23://printf("== %04X:%02X       RL    A ========\n",ADDR,OP0);  break;
		res1=(vACC<<1)|(vACC>>7);
		ACC.write(res1);
                break;
        case 0x24://printf("== %04X:%02X %02X    ADD   A,#%02Xh ========\n",ADDR,OP0,OP1,OP1);  break;
		res1=add(vACC,OP1,0);
		ACC.write(res1);
                break;
        case 0x25://printf("== %04X:%02X %02X    ADD   A,%02Xh ========\n",ADDR,OP0,OP1,OP1);  break;
		res2=read_direct(OP1,false);
		res1=add(vACC,res2,0);
		ACC.write(res1);
                break;
        case 0x26://printf("== %04X:%02X       ADD   A,@R%d ========\n",ADDR,OP0,aRn);  break;
		res2=read_indirect(REG[aRn].read());
		res1=add(vACC,res2,0);
		ACC.write(res1);
                break;
        case 0x28://printf("== %04X:%02X       ADD   A,R%d ========\n",ADDR,OP0,Rn);  break;
		res2=REG[Rn].read();
		res1=add(vACC,res2,0);
		ACC.write(res1);
                break;
        case 0x30://printf("== %04X:%02X %02X %02X JNB   %02Xh.%X,%04Xh ========\n",ADDR,OP0,OP1,OP2,BA,BIT,ADDRS3);  break;
		res1=read_bit(BA,BIT,false);
		if (!res1) {
		    jmp=true;
		    newPC=ADDRS3;
		}
                break;
        //case 0x31://printf("== %04X:%02X %02X    ACALL %04Xh ========\n",ADDR,OP0,OP1,ADDRA);  break;
        //        break;
        case 0x32://printf("== %04X:%02X       RETI ========\n",ADDR,OP0);  break;
		res1=intack.read();
		res2=res1&IP.read();
		if (res2==0) {
		    intack.write(0x00);
		    newPC=iret_low.read();
		} else {
		    intack.write(res1&~res2);
		    newPC=iret_high.read();
		}
		jmp=true;
                break;
        case 0x33://printf("== %04X:%02X       RLC   A ========\n",ADDR,OP0);  break;
		res1=(vACC<<1)|(vCF&0x01);
		CF.write(vACC>>7);
		ACC.write(res1);
                break;
        case 0x34://printf("== %04X:%02X %02X    ADDC  A,#%02Xh ========\n",ADDR,OP0,OP1,OP1);  break;
		res1=add(vACC,OP1,vCF);
		ACC.write(res1);
                break;
        case 0x35://printf("== %04X:%02X %02X    ADDC  A,%02Xh ========\n",ADDR,OP0,OP1,OP1);  break;
		res2=read_direct(OP1,false);
		res1=add(vACC,res2,vCF);
		ACC.write(res1);
                break;
        case 0x36://printf("== %04X:%02X       ADDC  A,@R%d ========\n",ADDR,OP0,aRn);  break;
		res2=read_indirect(REG[aRn].read());
		res1=add(vACC,res2,vCF);
		ACC.write(res1);
                break;
        case 0x38://printf("== %04X:%02X       ADDC  A,R%d ========\n",ADDR,OP0,Rn);  break;
		res2=REG[Rn].read();
		res1=add(vACC,res2,vCF);
		ACC.write(res1);
                break;
        case 0x40://printf("== %04X:%02X %02X    JC    %04Xh ========\n",ADDR,OP0,OP1,ADDRS2);  break;
		if (vCF) {
		    jmp=true;
		    newPC=ADDRS2;
		}
                break;
        //case 0x41://printf("== %04X:%02X %02X    AJMP  %04Xh ========\n",ADDR,OP0,OP1,ADDRA);  break;
        //        break;
        case 0x42://printf("== %04X:%02X %02X    ORL   %02Xh,A ========\n",ADDR,OP0,OP1,OP1);  break;
		res1=read_direct(OP1,true);
		res1|=vACC;
		write_direct(OP1,res1);
                break;
        case 0x43://printf("== %04X:%02X %02X %02X ORL   %02Xh,#%02Xh ========\n",ADDR,OP0,OP1,OP2,OP1,OP2);  break;
		res1=read_direct(OP1,true);
		res1|=OP2;
		write_direct(OP1,res1);
                break;
        case 0x44://printf("== %04X:%02X %02X    ORL   A,#%02Xh ========\n",ADDR,OP0,OP1,OP1);  break;
		res1=vACC;
		res1|=OP1;
		ACC.write(res1);
                break;
        case 0x45://printf("== %04X:%02X %02X    ORL   A,%02Xh ========\n",ADDR,OP0,OP1,OP1);  break;
		res1=vACC;
		res1|=read_direct(OP1,false);
		ACC.write(res1);
                break;
        case 0x46://printf("== %04X:%02X       ORL   A,@R%d ========\n",ADDR,OP0,aRn);  break;
		res1=vACC;
		res1|=read_indirect(REG[aRn].read());
		ACC.write(res1);
                break;
        case 0x48://printf("== %04X:%02X       ORL   A,R%d ========\n",ADDR,OP0,Rn);  break;
		res1=vACC;
		res1|=REG[Rn].read();
		ACC.write(res1);
                break;
        case 0x50://printf("== %04X:%02X %02X    JNC   %04Xh ========\n",ADDR,OP0,OP1,ADDRS2);  break;
		if (!vCF) {
		    jmp=true;
		    newPC=ADDRS2;
		}
                break;
        //case 0x51://printf("== %04X:%02X %02X    ACALL %04Xh ========\n",ADDR,OP0,OP1,ADDRA);  break;
        //        break;
        case 0x52://printf("== %04X:%02X %02X    ANL   %02Xh,A ========\n",ADDR,OP0,OP1,OP1);  break;
		res1=read_direct(OP1,true);
		res1&=vACC;
		write_direct(OP1,res1);
                break;
        case 0x53://printf("== %04X:%02X %02X %02X ANL   %02Xh,#%02Xh ========\n",ADDR,OP0,OP1,OP2,OP1,OP2);  break;
		res1=read_direct(OP1,true);
		res1&=OP2;
		write_direct(OP1,res1);
                break;
        case 0x54://printf("== %04X:%02X %02X    ANL   A,#%02Xh ========\n",ADDR,OP0,OP1,OP1);  break;
		res1=vACC;
		res1&=OP1;
		ACC.write(res1);
                break;
        case 0x55://printf("== %04X:%02X %02X    ANL   A,%02Xh ========\n",ADDR,OP0,OP1,OP1);  break;
		res1=vACC;
		res1&=read_direct(OP1,false);
		ACC.write(res1);
                break;
        case 0x56://printf("== %04X:%02X       ANL   A,@R%d ========\n",ADDR,OP0,aRn);  break;
		res1=vACC;
		res1&=read_indirect(REG[aRn].read());
		ACC.write(res1);
                break;
        case 0x58://printf("== %04X:%02X       ANL   A,R%d ========\n",ADDR,OP0,Rn);  break;
		res1=vACC;
		res1&=REG[Rn].read();
		ACC.write(res1);
                break;
        case 0x60://printf("== %04X:%02X %02X    JZ    %04Xh ========\n",ADDR,OP0,OP1,ADDRS2);  break;
		if (vACC==0) {
		    jmp=true;
		    newPC=ADDRS2;
		}
                break;
        //case 0x61://printf("== %04X:%02X %02X    AJMP  %04Xh ========\n",ADDR,OP0,OP1,ADDRA);  break;
        //        break;
        case 0x62://printf("== %04X:%02X %02X    XRL   %02Xh,A ========\n",ADDR,OP0,OP1,OP1);  break;
		res1=read_direct(OP1,true);
		res1^=vACC;
		write_direct(OP1,res1);
                break;
        case 0x63://printf("== %04X:%02X %02X %02X XRL   %02Xh,#%02Xh ========\n",ADDR,OP0,OP1,OP2,OP1,OP2);  break;
		res1=read_direct(OP1,true);
		res1^=OP2;
		write_direct(OP1,res1);
                break;
        case 0x64://printf("== %04X:%02X %02X    XRL   A,#%02Xh ========\n",ADDR,OP0,OP1,OP1);  break;
		res1=vACC;
		res1^=OP1;
		ACC.write(res1);
                break;
        case 0x65://printf("== %04X:%02X %02X    XRL   A,%02Xh ========\n",ADDR,OP0,OP1,OP1);  break;
		res1=vACC;
		res1^=read_direct(OP1,false);
		ACC.write(res1);
                break;
        case 0x66://printf("== %04X:%02X       XRL   A,@R%d ========\n",ADDR,OP0,aRn);  break;
		res1=vACC;
		res1^=read_indirect(REG[aRn].read());
		ACC.write(res1);
                break;
        case 0x68://printf("== %04X:%02X       XRL   A,R%d ========\n",ADDR,OP0,Rn);  break;
		res1=vACC;
		res1^=REG[Rn].read();
		ACC.write(res1);
                break;
        case 0x70://printf("== %04X:%02X %02X    JNZ   %04Xh ========\n",ADDR,OP0,OP1,ADDRS2);  break;
		if (vACC!=0) {
		    jmp=true;
		    newPC=ADDRS2;
		}
                break;
        //case 0x71://printf("== %04X:%02X %02X    ACALL %04Xh ========\n",ADDR,OP0,OP1,ADDRA);  break;
        //        break;
        case 0x72://printf("== %04X:%02X %02X    ORLB  C,%02Xh.%X ========\n",ADDR,OP0,OP1,BA,BIT);  break;
		res1=vCF;
		res1|=read_bit(BA,BIT,false);
		CF.write(res1);
                break;
        case 0x73://printf("== %04X:%02X       JMP   @A+DPTR ========\n",ADDR,OP0);  break;
		jmp=true;
		newPC=dptr+vACC;
                break;
        case 0x74://printf("== %04X:%02X %02X    MOV   A,#%02Xh ========\n",ADDR,OP0,OP1,OP1);  break;
		ACC.write(OP1);
                break;
        case 0x75://printf("== %04X:%02X %02X %02X MOV   %02Xh,#%02Xh ========\n",ADDR,OP0,OP1,OP2,OP1,OP2);  break;
		write_direct(OP1,OP2);
                break;
        case 0x76://printf("== %04X:%02X %02X    MOV   @R%d,#%02Xh ========\n",ADDR,OP0,OP1,aRn,OP1);  break;
		write_indirect(REG[aRn].read(),OP1);
                break;
        case 0x78://printf("== %04X:%02X %02X    MOV   R%d,#%02Xh ========\n",ADDR,OP0,OP1,Rn,OP1);  break;
		REG[Rn]=OP1;
                break;
        case 0x80://printf("== %04X:%02X %02X    SJMP  %04Xh ========\n",ADDR,OP0,OP1,ADDRS2);  break;		
		jmp=true;
		newPC=ADDRS2;
                break;
        //case 0x81://printf("== %04X:%02X %02X    AJMP  %04Xh ========\n",ADDR,OP0,OP1,ADDRA);  break;
        //        break;
        case 0x82://printf("== %04X:%02X %02X    ANLB  C,%02Xh.%X ========\n",ADDR,OP0,OP1,BA,BIT);  break;
		res1=vCF;
		res1&=read_bit(BA,BIT,false);
		CF.write(res1);
                break;
        case 0x83://printf("== %04X:%02X       MOVC  A,@A+PC ========\n",ADDR,OP0);  break;
		res1=rom[newPC+vACC];
		ACC.write(res1);
                break;
        case 0x84://printf("== %04X:%02X       DIV ========\n",ADDR,OP0);  break;
		if (BCC.read()==0) {
		    OV.write(1);
		} else {
		    OV.write(0);
		    res1=vACC/BCC.read();
		    res2=vACC%BCC.read();
		    ACC.write(res1);
		    BCC.write(res2);
		}
		CF.write(0);
                break;
        case 0x85://printf("== %04X:%02X %02X %02X MOV   %02Xh,%02Xh ========\n",ADDR,OP0,OP1,OP2,OP2,OP1);  break;
		if(OP1 != OP2)
		{
		res1=read_direct(OP1,false);
		write_direct(OP2,res1);
		}
		else
		{
		res1=read_direct(OP1,true);
		write_direct(OP2,res1);
		}
                break;
        case 0x86://printf("== %04X:%02X %02X    MOV   %02Xh,@R%d ========\n",ADDR,OP0,OP1,OP1,aRn);  break;
		res1=read_indirect(REG[aRn].read());
		write_direct(OP1,res1);
                break;
        case 0x88://printf("== %04X:%02X %02X    MOV   %02Xh,R%d ========\n",ADDR,OP0,OP1,OP1,Rn);  break;
		res1=REG[Rn].read();
		write_direct(OP1,res1);
                break;
        case 0x90://printf("== %04X:%02X %02X %02X MOV   DPTR,#%02X%02Xh ========\n",ADDR,OP0,OP1,OP2,OP1,OP2);  break;
		DPH.write(OP1);
		DPL.write(OP2);
		break;
        //case 0x91://printf("== %04X:%02X %02X    ACALL %04Xh ========\n",ADDR,OP0,OP1,ADDRA);  break;
        //        break;
        case 0x92://printf("== %04X:%02X %02X    MOVB  %02Xh.%X,C ========\n",ADDR,OP0,OP1,BA,BIT);  break;
		write_bit(BA,BIT,vCF);
                break;
        case 0x93://printf("== %04X:%02X       MOVC  A,@A+DPTR ========\n",ADDR,OP0);  break;
		res1=rom[dptr+vACC];
		ACC.write(res1);
                break;
        case 0x94://printf("== %04X:%02X %02X    SUBB  A,#%02Xh ========\n",ADDR,OP0,OP1,OP1);  break;
		res1=sub(vACC,OP1,vCF);
		ACC.write(res1);
                break;
        case 0x95://printf("== %04X:%02X %02X    SUBB  A,%02Xh ========\n",ADDR,OP0,OP1,OP1);  break;
		res1=read_direct(OP1,false);
		res1=sub(vACC,res1,vCF);
		ACC.write(res1);
                break;
        case 0x96://printf("== %04X:%02X       SUBB  A,@R%d ========\n",ADDR,OP0,aRn);  break;
		res1=read_indirect(REG[aRn].read());
		res1=sub(vACC,res1,vCF);
		ACC.write(res1);
                break;
        case 0x98://printf("== %04X:%02X       SUBB  A,R%d ========\n",ADDR,OP0,Rn);  break;
		res1=REG[Rn].read();
		res1=sub(vACC,res1,vCF);
		ACC.write(res1);
                break;
        case 0xA0://printf("== %04X:%02X %02X    ORLN  C,/%02Xh.%X ========\n",ADDR,OP0,OP1,BA,BIT);  break;
		res1=read_bit(BA,BIT,false);
		res1=vCF|~res1;
		CF.write(res1&0x01);
                break;
        //case 0xA1://printf("== %04X:%02X %02X    AJMP  %04Xh ========\n",ADDR,OP0,OP1,ADDRA);  break;
        //        break;
        case 0xA2://printf("== %04X:%02X %02X    MOVB  C,%02Xh.%X ========\n",ADDR,OP0,OP1,BA,BIT);  break;
		res1=read_bit(BA,BIT,false);
		CF.write(res1);
                break;
        case 0xA3://printf("== %04X:%02X       INC   DPTR ========\n",ADDR,OP0);  break;
		dptr=dptr+1;
		DPH.write(dptr>>8);
		DPL.write(dptr&0xFF);
                break;
        case 0xA4://printf("== %04X:%02X       MUL ========\n",ADDR,OP0);  break;
		resw=vACC*BCC.read();
		ACC.write(resw&0xFF);
		BCC.write(resw>>8);
		if ((resw&0xFF00)!=0) OV.write(1);
		else OV.write(0);
		CF.write(0);
                break;
        case 0xA5://printf("== %04X:%02X       MMX ========\n",ADDR,OP0);  break;
                break;
        case 0xA6://printf("== %04X:%02X %02X    MOV   @R%d,%02Xh ========\n",ADDR,OP0,OP1,aRn,OP1);  break;
		res1=read_direct(OP1,false);
		write_indirect(REG[aRn].read(),res1);
                break;
        case 0xA8://printf("== %04X:%02X %02X    MOV   R%d,%02Xh ========\n",ADDR,OP0,OP1,Rn,OP1);  break;
		res1=read_direct(OP1,false);
		REG[Rn].write(res1);
                break;
        case 0xB0://printf("== %04X:%02X %02X    ANLN  C,/%02Xh.%X ========\n",ADDR,OP0,OP1,BA,BIT);  break;
		res1=read_bit(BA,BIT,false);
		res1=vCF&~res1;
		CF.write(res1&0x01);
                break;
        //case 0xB1://printf("== %04X:%02X %02X    ACALL %04Xh ========\n",ADDR,OP0,OP1,ADDRA);  break;
        //        break;
        case 0xB2://printf("== %04X:%02X %02X    CPLB  %02Xh.%X ========\n",ADDR,OP0,OP1,BA,BIT);  break;
		res1=read_bit(BA,BIT,true);
		res1=(~res1)&0x01;
		write_bit(BA,BIT,res1);
                break;
        case 0xB3://printf("== %04X:%02X       CPLB  C ========\n",ADDR,OP0);  break;
		res1=vCF;
		CF.write((~res1)&0x01);
                break;
        case 0xB4://printf("== %04X:%02X %02X %02X CJNE  A,#%02Xh,%04Xh ========\n",ADDR,OP0,OP1,OP2,OP1,ADDRS3);  break;
		res1=sub(vACC,OP1,0);
		if (res1!=0) {
		    jmp=true;
		    newPC=ADDRS3;
		}
                break;
        case 0xB5://printf("== %04X:%02X %02X %02X CJNE  A,%02Xh,%04Xh ========\n",ADDR,OP0,OP1,OP2,OP1,ADDRS3);  break;
		res1=read_direct(OP1,false);
		res1=sub(vACC,res1,0);
		if (res1!=0) {
		    jmp=true;
		    newPC=ADDRS3;
		}
                break;
        case 0xB6://printf("== %04X:%02X %02X %02X CJNE  @R%d,#%02Xh,%04Xh ========\n",ADDR,OP0,OP1,OP2,aRn,OP1,ADDRS3);  break;
		res1=read_indirect(REG[aRn].read());
		res1=sub(res1,OP1,0);
		if (res1!=0) {
		    jmp=true;
		    newPC=ADDRS3;
		}
                break;
        case 0xB8://printf("== %04X:%02X %02X %02X CJNE  R%d,#%02Xh,%04Xh ========\n",ADDR,OP0,OP1,OP2,Rn,OP1,ADDRS3);  break;
		res1=REG[Rn].read();
		res1=sub(res1,OP1,0);
		if (res1!=0) {
		    jmp=true;
		    newPC=ADDRS3;
		}
                break;
        case 0xC0://printf("== %04X:%02X %02X    PUSH  %02Xh ========\n",ADDR,OP0,OP1,OP1);  break;
		res1=read_direct(OP1,false);
		vSP++;
		SP.write(vSP);
		write_indirect(vSP,res1);
                break;
        //case 0xC1://printf("== %04X:%02X %02X    AJMP  %04Xh ========\n",ADDR,OP0,OP1,ADDRA);  break;
        //        break;
        case 0xC2://printf("== %04X:%02X %02X    CLRB  %02Xh.%X ========\n",ADDR,OP0,OP1,BA,BIT);  break;
		write_bit(BA,BIT,0);
                break;
        case 0xC3://printf("== %04X:%02X       CLRB  C ========\n",ADDR,OP0);  break;
		CF.write(0);
                break;
        case 0xC4://printf("== %04X:%02X       SWAP  A ========\n",ADDR,OP0);  break;
		res1=vACC;
		res1=(res1>>4)|(res1<<4);
		ACC.write(res1);
                break;
        case 0xC5://printf("== %04X:%02X %02X    XCH   A,%02Xh ========\n",ADDR,OP0,OP1,OP1);  break;
		res1=vACC;
		res2=read_direct(OP1,false);
		ACC.write(res2);
		write_direct(OP1,res1);
                break;
        case 0xC6://printf("== %04X:%02X       XCH   A,@R%d ========\n",ADDR,OP0,aRn);  break;
		res1=vACC;
		res2=read_indirect(REG[aRn].read());
		ACC.write(res2);
		write_indirect(REG[aRn].read(),res1);
                break;
        case 0xC8://printf("== %04X:%02X       XCH   A,R%d ========\n",ADDR,OP0,Rn);  break;
		res1=vACC;
		res2=REG[Rn].read();
		ACC.write(res2);
		REG[Rn].write(res1);
                break;
        case 0xD0://printf("== %04X:%02X %02X    POP   %02Xh ========\n",ADDR,OP0,OP1,OP1);  break;
		res1=read_indirect(vSP);
		vSP--;
		SP.write(vSP);
		write_direct(OP1,res1);
                break;
        //case 0xD1://printf("== %04X:%02X %02X    ACALL %04Xh ========\n",ADDR,OP0,OP1,ADDRA);  break;
        //        break;
        case 0xD2://printf("== %04X:%02X %02X    SETB  %02Xh.%X ========\n",ADDR,OP0,OP1,BA,BIT);  break;
		write_bit(BA,BIT,1);
                break;
        case 0xD3://printf("== %04X:%02X       SETB  C ========\n",ADDR,OP0);  break;
		CF.write(1);
                break;
        case 0xD4://printf("== %04X:%02X       DA    A ========\n",ADDR,OP0);  break;
		res1=da(vACC,vCF,AC.read());
		ACC.write(res1);
                break;
        case 0xD5://printf("== %04X:%02X %02X %02X DJNZ  %02Xh,%04Xh ========\n",ADDR,OP0,OP1,OP2,OP1,ADDRS3);  break;
		res1=read_direct(OP1,true)-1;
		write_direct(OP1,res1);
		if (res1!=0) {
		    jmp=true;
		    newPC=ADDRS3;
		}
                break;
        case 0xD6://printf("== %04X:%02X       XCHD  A,@R%d ========\n",ADDR,OP0,aRn);  break;
		res1=vACC;
		res2=read_indirect(REG[aRn].read());
		ACC.write((res1&0xF0)|(res2&0x0F));
		write_indirect(REG[aRn].read(),((res1&0x0F)|(res2&0xF0)));
                break;
        case 0xD8://printf("== %04X:%02X %02X    DJNZ  R%d,%04Xh ========\n",ADDR,OP0,OP1,Rn,ADDRS2);  break;
		res1=REG[Rn].read()-1;
		REG[Rn].write(res1);
		if (res1!=0) {
		    jmp=true;
		    newPC=ADDRS2;
		}
                break;
        case 0xE0://printf("== %04X:%02X       MOVX  A,@DPTR ========\n",ADDR,OP0);  break;
		ACC.write(xram_rd_data.read());
		new_xram_re=true;
		new_xram_waitready=true;
                break;
        //case 0xE1://printf("== %04X:%02X %02X    AJMP  %04Xh ========\n",ADDR,OP0,OP1,ADDRA);  break;
        //        break;
        case 0xE2://printf("== %04X:%02X       MOVX  A,@R0 ========\n",ADDR,OP0);  break;
		ACC.write(xreg_rd_data);
		xreg_addr_sel.write(0);
		new_xreg_re=true;
		new_xreg_waitready=true;
                break;
        case 0xE3://printf("== %04X:%02X       MOVX  A,@R1 ========\n",ADDR,OP0);  break;
		ACC.write(xreg_rd_data);
		xreg_addr_sel.write(1);
		new_xreg_re=true;
		new_xreg_waitready=true;
                break;
        case 0xE4://printf("== %04X:%02X       CLR   A ========\n",ADDR,OP0);  break;
		ACC.write(0x00);
                break;
        case 0xE5://printf("== %04X:%02X %02X    MOV   A,%02Xh ========\n",ADDR,OP0,OP1,OP1);  break;
		res1=read_direct(OP1,false);
		ACC.write(res1);
                break;
        case 0xE6://printf("== %04X:%02X       MOV   A,@R%d ========\n",ADDR,OP0,aRn);  break;
		res1=read_indirect(REG[aRn].read());
		ACC.write(res1);
                break;
        case 0xE8://printf("== %04X:%02X       MOV   A,R%d ========\n",ADDR,OP0,Rn);  break;
		res1=REG[Rn].read();
		ACC.write(res1);
                break;
        case 0xF0://printf("== %04X:%02X       MOVX  @DPTR,A ========\n",ADDR,OP0);  break;
		new_xram_we=true;
		new_xram_waitready=true;
                break;
        //case 0xF1://printf("== %04X:%02X %02X    ACALL %04Xh ========\n",ADDR,OP0,OP1,ADDRA);  break;
        //        break;
        case 0xF2://printf("== %04X:%02X       MOVX  @R0,A ========\n",ADDR,OP0);  break;
		xreg_addr_sel.write(0);
		new_xreg_we=true;
		new_xreg_waitready=true;
                break;
        case 0xF3://printf("== %04X:%02X       MOVX  @R1,A ========\n",ADDR,OP0);  break;
		xreg_addr_sel.write(1);
		new_xreg_we=true;
		new_xreg_waitready=true;
                break;
        case 0xF4://printf("== %04X:%02X       CPL   A ========\n",ADDR,OP0);  break;
		res1=vACC;
		ACC.write(~res1);
                break;
        case 0xF5://printf("== %04X:%02X %02X    MOV   %02Xh,A ========\n",ADDR,OP0,OP1,OP1);  break;
		res1=vACC;
		write_direct(OP1,res1);
                break;
        case 0xF6://printf("== %04X:%02X       MOV   @R%d,A ========\n",ADDR,OP0,aRn);  break;
		res1=vACC;
		write_indirect(REG[aRn].read(),res1);
                break;
        case 0xF8://printf("== %04X:%02X       MOV   R%d,A ========\n",ADDR,OP0,Rn);  break;
		res1=vACC;
		REG[Rn].write(res1);
                break;
        }
    }
    PC.write(newPC);
    xreg_re.write(new_xreg_re);
    xreg_we.write(new_xreg_we);
    xram_re.write(new_xram_re);
    xram_we.write(new_xram_we);
    xreg_waitready.write(new_xreg_waitready);
    xram_waitready.write(new_xram_waitready);
}


void    core_8051::save_vars(VARS_8051 *dst)
{
    int i;
    dst->port0o=port0o.read();
    dst->port1o=port1o.read();
    dst->port2o=port2o.read();
    dst->port3o=port3o.read();
    dst->ACC   =ACC   .read();
    dst->BCC   =BCC   .read();
    dst->IE    =IE    .read();
    dst->IP    =IP    .read();
    dst->SP    =SP    .read();
    dst->DPH   =DPH   .read();
    dst->DPL   =DPL   .read();
    dst->TCON  =TCON  .read();
    dst->TL0   =TL0   .read();
    dst->TL1   =TL1   .read();
    dst->TH0   =TH0   .read();
    dst->TH1   =TH1   .read();
    dst->TU0   =TU0   .read();
    dst->TU1   =TU1   .read();
    dst->RCAPL0=RCAPL0.read();
    dst->RCAPL1=RCAPL1.read();
    dst->RCAPH0=RCAPH0.read();
    dst->RCAPH1=RCAPH1.read();
    dst->RCAPU0=RCAPU0.read();
    dst->RCAPU1=RCAPU1.read();
    dst->CF    =CF    .read();
    dst->AC    =AC    .read();
    dst->OV    =OV    .read();
    dst->pidle =pidle .read();
    dst->jmp   =jmp;
    dst->newPC =PC.get_new_value();
    for(i=0;i<8;i++) dst->REG[i]=REG[i].read();
    for(i=0;i<256;i++) dst->ram[i]=ram[i];
//    dst->PLX_P =plx.P;
//    for(i=0;i<8;i++) dst->PLX_R[i]=plx.R[i];
}

void    show_debug(const VARS_8051 *news, const VARS_8051 *olds)
{
    int i;
    if (news->port0o!=olds->port0o) printf("  --port0o :old=%02X new=%02X\n",olds->port0o,news->port0o);
    if (news->port1o!=olds->port1o) printf("  --port1o :old=%02X new=%02X\n",olds->port1o,news->port1o);
    if (news->port2o!=olds->port2o) printf("  --port2o :old=%02X new=%02X\n",olds->port2o,news->port2o);
    if (news->port3o!=olds->port3o) printf("  --port3o :old=%02X new=%02X\n",olds->port3o,news->port3o);
    if (news->ACC   !=olds->ACC   ) printf("  --ACC    :old=%02X new=%02X\n",olds->ACC   ,news->ACC   );
    if (news->BCC   !=olds->BCC   ) printf("  --BCC    :old=%02X new=%02X\n",olds->BCC   ,news->BCC   );
    if (news->IE    !=olds->IE    ) printf("  --IE     :old=%02X new=%02X\n",olds->IE    ,news->IE    );
    if (news->IP    !=olds->IP    ) printf("  --IP     :old=%02X new=%02X\n",olds->IP    ,news->IP    );
    if (news->SP    !=olds->SP    ) printf("  --SP     :old=%02X new=%02X\n",olds->SP    ,news->SP    );
    if (news->DPH   !=olds->DPH   ) printf("  --DPH    :old=%02X new=%02X\n",olds->DPH   ,news->DPH   );
    if (news->DPL   !=olds->DPL   ) printf("  --DPL    :old=%02X new=%02X\n",olds->DPL   ,news->DPL   );
    if (news->TCON  !=olds->TCON  ) printf("  --TCON   :old=%02X new=%02X\n",olds->TCON  ,news->TCON  );
    if (news->TL0   !=olds->TL0   ) printf("  --TL0    :old=%02X new=%02X\n",olds->TL0   ,news->TL0   );
    if (news->TL1   !=olds->TL1   ) printf("  --TL1    :old=%02X new=%02X\n",olds->TL1   ,news->TL1   );
    if (news->TH0   !=olds->TH0   ) printf("  --TH0    :old=%02X new=%02X\n",olds->TH0   ,news->TH0   );
    if (news->TH1   !=olds->TH1   ) printf("  --TH1    :old=%02X new=%02X\n",olds->TH1   ,news->TH1   );
    if (news->TU0   !=olds->TU0   ) printf("  --TU0    :old=%02X new=%02X\n",olds->TU0   ,news->TU0   );
    if (news->TU1   !=olds->TU1   ) printf("  --TU1    :old=%02X new=%02X\n",olds->TU1   ,news->TU1   );
    if (news->TL0   !=olds->RCAPL0) printf("  --RCAPL0 :old=%02X new=%02X\n",olds->RCAPL0,news->RCAPL0);
    if (news->TL1   !=olds->RCAPL1) printf("  --RCAPL1 :old=%02X new=%02X\n",olds->RCAPL1,news->RCAPL1);
    if (news->TH0   !=olds->RCAPH0) printf("  --RCAPH0 :old=%02X new=%02X\n",olds->RCAPH0,news->RCAPH0);
    if (news->TH1   !=olds->RCAPH1) printf("  --RCAPH1 :old=%02X new=%02X\n",olds->RCAPH1,news->RCAPH1);
    if (news->TU0   !=olds->RCAPU0) printf("  --RCAPU0 :old=%02X new=%02X\n",olds->RCAPU0,news->RCAPU0);
    if (news->TU1   !=olds->RCAPU1) printf("  --RCAPU1 :old=%02X new=%02X\n",olds->RCAPU1,news->RCAPU1);
    if (news->CF    !=olds->CF    ) printf("  --CF     :old=%X  new=%X\n",olds->CF    ,news->CF    );
    if (news->AC    !=olds->AC    ) printf("  --AC     :old=%X  new=%X\n",olds->AC    ,news->AC    );
    if (news->OV    !=olds->OV    ) printf("  --OV     :old=%X  new=%X\n",olds->OV    ,news->OV    );
    if (news->pidle !=olds->pidle ) printf("  --pidle  :old=%X  new=%X\n",olds->pidle ,news->pidle );
    for(i=0;i<8;i++) {
        if (news->REG[i]!=olds->REG[i]) printf("  --R%d :old=%02X new=%02X\n",i,olds->REG[i],news->REG[i]);
    }
    for(i=0;i<256;i++) {
        if (news->ram[i]!=olds->ram[i]) printf("  --ram[%02X]:old=%02X new=%02X\n",i,olds->ram[i],news->ram[i]);
    }
    if (news->jmp) {
	printf("  --Jump %04X\n",news->newPC);
    }
    if ((news->PLX_P&0x02) !=(olds->PLX_P&0x02) ) printf("  --PLX_P1  :old=%X  new=%X\n",((olds->PLX_P>>1)&0x01) ,((news->PLX_P>>1)&0x01) );
    if ((news->PLX_P&0x04) !=(olds->PLX_P&0x04) ) printf("  --PLX_P2  :old=%X  new=%X\n",((olds->PLX_P>>2)&0x01) ,((news->PLX_P>>2)&0x01) );
    if ((news->PLX_P&0x08) !=(olds->PLX_P&0x08) ) printf("  --PLX_P3  :old=%X  new=%X\n",((olds->PLX_P>>3)&0x01) ,((news->PLX_P>>3)&0x01) );
    for(i=0;i<8;i++) {
        if (news->PLX_R[i]!=olds->PLX_R[i]) printf("  --PLX_R%d :old=%08lX new=%08lX\n",i,olds->PLX_R[i],news->PLX_R[i]);
    }

}
