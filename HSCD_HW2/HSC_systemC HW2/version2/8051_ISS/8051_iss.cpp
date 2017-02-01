#include "systemc.h"
#include <conio.h>
#include <stdlib.h>
#define clockcycle      10  //10ns=100MHz
//#define clockcycle_800  1.25//1.25ns=800MHz
#include "top.h"


DWORD WINAPI ShowWinThreadProc(LPVOID pParam);
DWORD ShowWinID=0;



int sc_main(int argc, char *argv[])
{
	// remove warnings
	sc_report_handler::set_actions("/IEEE_Std_1666/deprecated", SC_DO_NOTHING);

	char *filename;
	bool dolog;

	if (argc<2) {
		printf("Usage:8051_iss filename imagename\n");
		return 1;
	}
	if (argc<3) {
		printf("Usage:8051_iss filename imagename\n");
		return 1;
	}
	filename=argv[1];

	sc_clock                clk("CLOCK",clockcycle,0.5,1,SC_NS);
	//sc_clock                clk_800("CLOCK_800",clockcycle_800,0.5,1,SC_NS);
	sc_signal<sc_uint<1> >  reset;
	sc_signal<sc_uint<1> >  poff;
	top top("top");
	top.clk(clk);
    //top.clk(clk_800);
	top.reset(reset);
	top.poff(poff);

	if (!top.core->loadrom(filename)) {
		printf("%s file not found\n",filename);
		system("pause");
		return 1;
	}
	filename=argv[2];

	{
		FILE *FR;
		unsigned char s;
		int i;
		FR=fopen(filename,"rb");
		if (FR==NULL) {printf("%s file not found\n",filename);return 1;}
		for(i=0;i<8*8;i++)
		{
			s=fgetc(FR);
			top.xrambig->mem[(0x3000)+i]=s;
		}
		fclose (FR);
	}

	//Start Test
	int t,k;
	int score=0;
	VARS_8051   olds, news;

	reset.write(1);

	for(t=0;t<10;t++) {
		sc_start(clockcycle,SC_NS);
	}
	reset.write(0);
	top.core->save_vars(&olds);
	//dolog=true;
	dolog=false;
	for(;t<300;t++) {
		if (dolog) {
			printf("%08d#",t);
			top.core->show_assembly();
		}

		if(t % 1 == 0){
			int p0 = top.port0o.read();  //A 8051送出sample值
			int p1 = top.port1i.read();  //D FIR回傳計算結果
			int p2 = top.port3i.read();  //C 1:FIR已把結果計算完畢  0:FIR結果尚未計算完畢
			int p3 = top.port3o.read();  //B 1:8051把sample值準備好 0:8051的sample值尚未準備好

			printf("Time:%d, P0=%d, P1=%d, P2=%d, P3=%d\n\n", t, p0, p1, p2, p3);
		}

		sc_start(clockcycle,SC_NS);
		if (dolog) 
		{
			top.core->save_vars(&news);
			show_debug(&news, &olds);
			memcpy(&olds,&news,sizeof(VARS_8051));
		}

		// Start signal
		if(t==300)
		{
			top.port0i.write(170);
		}

/*
		if (poff.read()!=0)
		{
			if(top.port2o.read()==2)
				if(top.port3o.read()==0)
				{
					printf("correct\n");
					break;
				}
			printf("wrong\n");
			break;
		}


		if(t==10)
		{
			top.port0i.write(16);
			top.port1i.write(32);
			printf("Write data to P0 and P1\n");
		}

		if(t==39990)
		{
			if(top.port2o.read()==2)
				if(top.port3o.read()==0)
				{
					printf("correct\n");
					break;
				}
			printf("wrong\n");
			break;
		}
*/
		// Press "Esc" to exit this program
		if (kbhit()) 
		{
			k=getch();
			if ((k==3)||(k==27)) break;
		}
	}
	system("pause");
	return 0;
}
