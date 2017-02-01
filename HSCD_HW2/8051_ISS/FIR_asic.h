#ifndef	__ASIC_H
#define __ASIC_H
#include "systemc.h"
#include <stdio.h>
SC_MODULE(FIR_asic)
{
	sc_in_clk		clk;
	sc_in<sc_uint<8> >	A;  //P0
	sc_in<sc_uint<8> >	B;  //P3
	sc_out<sc_uint<8> >	C;  //P2
	sc_out<sc_uint<8> >	D;  //P1
	SC_CTOR(FIR_asic)
	{
		SC_CTHREAD(behavior, clk.neg());
	}
private:
	
	void behavior() {
		const int FIR_LEN = 7;
		const int c[FIR_LEN] = {1, 2, 3, 4, 3, 2, 1};
		int i[FIR_LEN-1] = {0};
		C.write(0);
		while(1){
			wait();
			//cout << A.read() << ", " << B.read() << ", " << C.read() << ", " << D.read() << endl;
			//getchar();
			if(B.read() == 1){
				//printf("in\n");
				D.write( 
					c[0] * A.read() + 
					c[1] * i[0] +
					c[2] * i[1] +
					c[3] * i[2] +
					c[4] * i[3] +
					c[5] * i[4] +
					c[6] * i[5]
				);

				i[6] = i[5];
				i[5] = i[4];
				i[4] = i[3];
				i[3] = i[2];
				i[2] = i[1];
				i[1] = i[0];
				i[0] = A.read();
		
				C.write(1);
			}
			else{
				C.write(0);
			}
		}
	}
	/*
	void behavior() {
		const int FIR_LEN = 7;
		const int c[FIR_LEN] = {1, 2, 3, 4, 3, 2, 1};
		int i[FIR_LEN-1] = {0};
		C.write(0);
		while(1){
			wait();
			//cout << A.read() << ", " << B.read() << ", " << C.read() << ", " << D.read() << endl;
			//getchar();
			if(B.read() == 1){
				//printf("in\n");
				i[0] = A.read();
				sc_uint<8> sum = 0;

				for(int j = 0; j < FIR_LEN; ++j)
					sum = sum + c[j] * i[j];

				D.write(sum);

				for(int j = FIR_LEN - 1; j != 0; --j)
					i[j] = i[j-1];
				
				C.write(1);
			}
			else{
				C.write(0);
			}
		}
	}
	*/
	
};

#endif
