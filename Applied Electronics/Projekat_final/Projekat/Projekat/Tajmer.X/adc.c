#include<p30fxxxx.h>
#include "adc.h"

void ConfigureADCPins(void)
{
	ADPCFGbits.PCFG8=0;
	ADPCFGbits.PCFG9=0;
	
	TRISBbits.TRISB8=1;
	TRISBbits.TRISB9=1;
}

void ADCinit(void)
{


ADCON1bits.ADSIDL=0;
ADCON1bits.FORM=0;
ADCON1bits.SSRC=7;

ADCON1bits.SAMP=1;

ADCON2bits.VCFG=7; 
ADCON2bits.CSCNA=1;
ADCON2bits.SMPI=3;
ADCON2bits.BUFM=0;
ADCON2bits.ALTS=0;



ADCON3bits.SAMC=31;
ADCON3bits.ADRC=1;
ADCON3bits.ADCS=31;


ADCHSbits.CH0NB=0;
ADCHSbits.CH0NA=0;

ADCHSbits.CH0SA=0;
ADCHSbits.CH0SB=0;


ADPCFGbits.PCFG6=1;
ADPCFGbits.PCFG8=0;
ADPCFGbits.PCFG9=0;
ADPCFGbits.PCFG7=0;
ADPCFGbits.PCFG12=0;


ADCSSL=0b0001001110000000; //12, 8, 9, 7 = mq3,touch,touch,fotootpornik 
ADCON1bits.ASAM=1;

IFS0bits.ADIF=1;
IEC0bits.ADIE=1;
ADCON1bits.ADON = 1;
}



