#include <p30fxxxx.h>
#include "tajmeri.h"

#define TMR2_period 333 /*  Fosc = 3.33MHz,
							  1/Fosc = 0.3us !!!, 0.3us * 333 = 1ms  
                              Delay_ms(3000); je 1 sekund  
*/


void Init_T2(void) //Delay_ms
{
	T2CONbits.TON = 0;	// T2 off 
	TMR2 = 0;			//clear timer register
	PR2 = TMR2_period;	//
	
	T2CONbits.TCS = 0; // 0 = Internal clock (FOSC/4)
	//IPC1bits.T2IP = 0; // T2 prioritet 0-najmanje 1 je srednje 2 je najveci
	IFS0bits.T2IF = 0; // clear interrupt flag
	IEC0bits.T2IE = 1; // enable interrupt
	T2CONbits.TON = 1; // T2 on 
}
