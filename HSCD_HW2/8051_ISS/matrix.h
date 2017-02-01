#include "systemc.h"
extern int key[49][6];
extern bool spe[49][3];
SC_MODULE(matrix)
{
	sc_in<sc_uint<3> >  row; 
	sc_in_clk			clk;	
	sc_out<sc_uint<5> > column;
	sc_out<bool>		shift;
	sc_out<bool>		ctrl;
	sc_out<bool>		alt;
	SC_CTOR(matrix)
	{
		counter=0;
		pricounter=0;
		SC_METHOD(lookup2);
		sensitive<<row;
		SC_METHOD(count);
		sensitive << clk.pos();
	}
private:
	sc_signal<int>		state;
	int					counter;
	int					pricounter;

	void lookup2()
	{
		if(counter!=pricounter)
		{
			column=key[state][row.read()-1];
			shift=spe[state][0];
			ctrl=spe[state][1];
			alt=spe[state][2];
		}
		pricounter=counter;
	} 
	void count()
	{
		counter+=1;
		switch(counter)
		{
			case 10:state=0;break;
			case 1000:state=1;break;
			case 1500:state=0;break;
			case 2000:state=2;break;
			case 2500:state=0;break;
			case 3000:state=3;break;
			case 3500:state=0;break;
			case 4000:state=4;break;
			case 4500:state=0;break;
			case 5000:state=5;break;
			case 5500:state=0;break;
			case 6000:state=6;break;
			case 6500:state=0;break;
			case 7000:state=7;break;
			case 7500:state=0;break;
			case 8000:state=8;break;
			case 8500:state=0;break;
			case 9000:state=9;break;
			case 9500:state=0;break;

			case 20000:state=10;break;
			case 20500:state=0;break;
			case 21000:state=11;break;
			case 21500:state=0;break;
			case 22000:state=12;break;
			case 22500:state=0;break;
			case 23000:state=13;break;
			case 23500:state=0;break;
			case 24000:state=14;break;
			case 24500:state=0;break;
			case 25000:state=15;break;
			case 25500:state=0;break;

			case 40000:state=16;break;
			case 40500:state=0;break;
			case 41000:state=16;break;
			case 41500:state=0;break;
			case 42000:state=17;break;
			case 42500:state=0;break;
			case 43000:state=18;break;
			case 43500:state=0;break;
			case 44000:state=19;break;
			case 44500:state=0;break;
			case 45000:state=20;break;
			case 45500:state=0;break;
			case 46000:state=21;break;
			case 46500:state=0;break;
			case 47500:state=22;break;
			case 48000:state=0;break;
			case 48500:state=23;break;
			case 49000:state=0;break;
			case 49500:state=24;break;
			case 50000:state=0;break;
			case 50500:state=25;break;
			case 51000:state=0;break;
			case 51500:state=26;break;
			case 52000:state=0;break;
			default:;
		}
	}
};
