#include <io51.h>

void init_iss(void){
  //P0=85;
  //while(P0!=170);
  //P2=0;
  //P3=0;
  //P0=0;
  //P0=150;
  P0=0;    //out, output random number to FIR
  P1=0;    //in , input the result of FIR
  P2=0;    //in , P2=0 -> the result of FIR isn't ready
           //     P2=1 -> the result of FIR is ready
  P3=0;    //out, P3=0 -> FIR can't read the number
           //     P3=1 -> FIR can read the number
}

void main(void)
{
  init_iss();
  
  while(1){
  	P0=1;
  	P3=1;
  	//PCON = 2;         // Set a power down control signal to 8051
  	while(P3 == 1){
		;
  	}
        P0=0;
        P3=0;
  }  
}

