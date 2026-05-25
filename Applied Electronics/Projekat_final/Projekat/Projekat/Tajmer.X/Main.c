#include <p30fxxxx.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "driverGLCD.h"
#include "adc.h"
#include "tajmeri.h"

#define DRIVE_A LATCbits.LATC13
#define DRIVE_B LATCbits.LATC14

_FOSC(CSW_FSCM_OFF & HS3_PLL4);
_FWDT(WDT_OFF);
_FGS(CODE_PROT_OFF);

//==========================================DEKLARACIJE PROMENLJIVIH=================================================================================================

typedef enum {
    STATE_DISARMED,
    STATE_ARMED,
    STATE_PIR_ACTIVATED,
    STATE_MQ3_ACTIVATED
} GLCD_STANJE;

GLCD_STANJE currentState;
GLCD_STANJE previousState;

unsigned int brojac_ms = 0,stoperica,ms,sekund;
unsigned int sirovi0, sirovi1,sirovi2,sirovi3, tempRX;
int stanje=0;
int broj1, broj2;
unsigned int X, Y,x_vrednost, y_vrednost; //GLCD
unsigned int temp0,temp1;

#define BUFFER_LEN 16
volatile char uart_rx_buffer[BUFFER_LEN];
volatile int uart_rx_index = 0;
volatile bool uart_command_ready = false;

const unsigned int AD_Xmin =220;
const unsigned int AD_Xmax =3642;
const unsigned int AD_Ymin =520;
const unsigned int AD_Ymax =3450;

//==========================================PREKIDI=================================================================================================
void __attribute__ ((__interrupt__)) _T2Interrupt(void) // svakih 1ms - Delay_ms je tajmer 2
{
     TMR2 =0; //resetuje tajmer

	brojac_ms++;//brojac milisekundi
    stoperica++;//brojac za funkciju Delay_ms

    if (brojac_ms==1000)//sek
        {
          brojac_ms=0;
          sekund=1;//fleg za sekundu
		 } 
	IFS0bits.T2IF = 0;  //clearujemo fleg
       
}

void __attribute__((__interrupt__)) _U1RXInterrupt(void) //mozda uopste ne valja kopirano je 
{
    IFS0bits.U1RXIF = 0;
    if (U1STAbits.OERR) U1STAbits.OERR = 0; //mozda ne treba-trebalo bi da cisti bafer


    char received = U1RXREG;

    if (!uart_command_ready) {
        if (received == '\n' || received == '\r') {
            uart_rx_buffer[uart_rx_index] = '\0';  // Null-terminate
            uart_rx_index = 0;
            uart_command_ready = true;
        }
        else if (uart_rx_index < BUFFER_LEN - 1) {
            uart_rx_buffer[uart_rx_index++] = received;
        }
        else {
            uart_rx_index = 0; //za overflow  
        }
    }
} 

void __attribute__((__interrupt__)) _ADCInterrupt(void) 
{
	sirovi0=ADCBUF0; //fotootpornik
	sirovi1=ADCBUF1; //Touch
    sirovi2=ADCBUF2; //Touch
    sirovi3=ADCBUF3; //MQ3
    	
    temp0=sirovi1; // X koordinata
	temp1=sirovi2; // Y koordinata
    
    IFS0bits.ADIF = 0;

} 

//==========================================DELAY====================================================================================================

void Delay_ms (int vreme)//funkcija za kasnjenje u milisekundama
	{
		stoperica = 0;
		while(stoperica < vreme);
	}

void delay(int pom){
    
    for(broj1=0;broj1<pom;broj1++)
                for(broj2=0;broj2<60;broj2++);
}

//==========================================KONFIGURACIJA MAIN-A=================================================================================================

void init_main(){

    ConfigureLCDPins();
	ConfigureTSPins();
	GLCD_LcdInit();
	GLCD_ClrScr();
    ConfigureADCPins();
    
    initUART1();
    ADCinit();
    Init_T2();
    
    ADCON1bits.ADON=1;
	TRISAbits.TRISA11=0;  //izlaz za buzzer na A11
    TRISBbits.TRISB12=1;   //ulaz za mq3 na B12
    TRISBbits.TRISB6=1;   //ulaz za pir na B6
    TRISBbits.TRISB7=1;   //ulaz za FOTOOTPORNIK na B10
    TRISBbits.TRISB11=0;  //izlazni PWM za servo na B11
    T2CONbits.TON=1;
}

//==========================================FUNKCIJE ZA RAD SA SENZORIMA I AKTUATORIMA====================================================================================


void ConfigureTSPins(void) //touch screen
{
	//ADPCFGbits.PCFG10=1;
	//ADPCFGbits.PCFG7=digital;

	//TRISBbits.TRISB10=0;
	TRISCbits.TRISC13=0;
    TRISCbits.TRISC14=0;
	
	//LATCbits.LATC14=0;
	//LATCbits.LATC13=0;
}

void Write_GLCD(unsigned int data)
{
unsigned char temp;

temp=data/10;
Glcd_PutChar(temp+'0');
data=data-temp*10;
Glcd_PutChar(data+'0');
}

void Touch_Panel (void)
{
// vode horizontalni tranzistori
	DRIVE_A = 1;  
	DRIVE_B = 0;
                
	Delay_ms(100); //cekamo jedno vreme da se odradi AD konverzija - Sto je veci kod iz nekog razloga mu treba veci delay
				
	// ocitavamo x	
	x_vrednost = temp0;//temp0 je vrednost koji nam daje AD konvertor na BOTTOM pinu		

// vode vertikalni tranzistori

	DRIVE_A = 0;  
	DRIVE_B = 1;

	Delay_ms(100); //cekamo jedno vreme da se odradi AD konverzija
	
	// ocitavamo y	
	y_vrednost = temp1;// temp1 je vrednost koji nam daje AD konvertor na LEFT pinu	
    
    //skaliranje x-koordinate

    X=(x_vrednost-161)*0.03629;

    //Skaliranje Y-koordinate
	Y= ((y_vrednost-500)*0.020725);

}

void initUART1(void)     //Namestamo uart
{
    U1STAbits.OERR = 0; //mozda ne treba trebalo bi da cisti bafer

U1BRG=0x0015;//baud rate 9600
U1MODEbits.ALTIO = 0;
IEC0bits.U1RXIE = 1; //Omogucavamo interapt
U1STA&=0xfffc;
U1MODEbits.UARTEN=1;
U1STAbits.UTXEN=1;
IFS0bits.U1RXIF = 0; // clear interrupt flag
}

void WriteUART1(unsigned int data) //ispisuje jedan karakter
{
	while (U1STAbits.TRMT==0);
    if(U1MODEbits.PDSEL == 3)
        U1TXREG = data;
    else
        U1TXREG = data & 0xFF;
}

void WriteUART1dec2string(unsigned int data) //ispis ad vrednosti
{
	unsigned char temp;

	temp=data/1000;
	WriteUART1(temp+'0');
	data=data-temp*1000;
	temp=data/100;
	WriteUART1(temp+'0');
	data=data-temp*100;
	temp=data/10;
	WriteUART1(temp+'0');
	data=data-temp*10;
	WriteUART1(data+'0');
}

 void RS232_putst(register const char *str) //Sluzi za ispis resenja
 {
   while((*str)!=0)
   {
   WriteUART1(*str);
   str++;
  }
  }
 
 void Zujalica(int jedinica, int nula, int frekv){ //frekvencija u rasponu 
int k=0;

while(k<100){
        int f =0;
        
        while(f < frekv){
        LATAbits.LATA11= 1;
        delay(jedinica);
        LATAbits.LATA11 = 0;
        delay(nula);
                f++;
               }
        k++;
     }
}
 
 
  void Servo_drugi_smer(){ //Ukupni zbir delaya mora bit 134, delay za 1 treba da bude izmedju 5 i 12
       
     int p=0;
     while(p<15){
     LATBbits.LATB11 = 1;
        delay(6);
        LATBbits.LATB11 = 0;
        delay(112);
        p++;
     }
}
 
 void Servo_jedan_smer(){ //Ukupni zbir delaya mora bit 134, delay za 1 treba da bude izmedju 5 i 12
       
     int p=0;
     while(p<15){
     LATBbits.LATB11 = 1;
        delay(12);
        LATBbits.LATB11 = 0;
        delay(112);
        p++;
     }
}
 
 unsigned int PIRCheck (){  //Proverava stanje na piru
    unsigned int pirsirovi = PORTBbits.RB6;
    return pirsirovi;
}

unsigned int MQ3_check(){ //Provera stanja na mq3
    unsigned int mq3sirovi = sirovi3;
    return mq3sirovi;
 }

unsigned int scale_light(unsigned int fo) {
    if (fo > 4095) fo = 4095;
    return ((unsigned long) fo * 100 + 2047) / 4095;
}

void dan_noc(){
    int fread=sirovi0;
    unsigned int light_percent = scale_light(fread);
    
    
    if (light_percent > 60) {
        RS232_putst("Dobar dan");
        delay(3000);
        WriteUART1(13);
    }
    else {
        RS232_putst("Dobro vece");
        delay(3000);
        WriteUART1(13);
    }
}


//==========================================STANJA=================================================================================

void check_state(void) {
    if (currentState != previousState) {
        previousState = currentState;
    }
}

void handle_state(void) {

    switch (currentState) {
        case STATE_DISARMED:
            handle_home_display();
            break;
        case STATE_ARMED:
            handle_armed_display();
            break;
        case STATE_MQ3_ACTIVATED:
            handle_MQ3_display();
            break;
        case STATE_PIR_ACTIVATED:
            handle_PIR_display();
            break;
        default:
            handle_home_display();
            break;
    }
}

void handle_home_display(void) {
    GLCD_ClrScr();
    int x1_vrednost =5;
    int x2_vrednost =107;
    int y1_vrednost =20;
    int y2_vrednost =35;
    
    
    GoToXY(12,3);		
		GLCD_Printf ("Naoruzaj sistem");
        GLCD_Rectangle(x1_vrednost, y1_vrednost, x2_vrednost, y2_vrednost);     
        
    Zujalica(5,5,1); 
    Delay_ms(30);
        
    while(currentState==STATE_DISARMED){
    Touch_Panel();                 
        if(X>x1_vrednost &&  X<x2_vrednost && Y>y1_vrednost+10 && Y<y2_vrednost+10){
            Delay_ms(30);
            currentState=STATE_ARMED;        
        }
    }
}

void handle_armed_display(void) {
    int mq3, pir, photores;

        GLCD_ClrScr();
        GoToXY(12,3);		
		GLCD_Printf ("Sistem naoruzan");
        
        Zujalica(3,7,2);
        Delay_ms(30);
        
        mq3=MQ3_check();      
        pir=PIRCheck();
        
        dan_noc();
        
        if(mq3>=3000){                          //MQ3 ocitaj      
        currentState=STATE_MQ3_ACTIVATED;   
         GLCD_Rectangle(20, 20 , 20, 20);
        }
        else if(pir==1){                        //PIR 1-ocitao je pokret, 0 nije ocitao pokret
        currentState= STATE_PIR_ACTIVATED;
        }
}

void handle_MQ3_display(void) {
    int i=5;
        
    Servo_jedan_smer();
    Zujalica(1,9,3);
    Delay_ms(30);
        
    for(i; i>=0; i--){
        GLCD_ClrScr();
        GoToXY(12,3);
        GLCD_Printf ("Automatsko gasenje");
        GoToXY(17,4);		
		GLCD_Printf ("za:");
        GoToXY(35,4);
        Write_GLCD(i);
        Delay_ms(3000);
        Zujalica(4,6,3);
        
    }

    currentState=STATE_DISARMED;
}

void handle_PIR_display(void) {
    GLCD_ClrScr();
    int x1_vrednost =20;
    int x2_vrednost =100;
    int y1_vrednost =35;
    int y2_vrednost =50;
    
    Servo_drugi_smer();
    Delay_ms(30);
        
    
    GoToXY(12,1);		
	GLCD_Printf ("Manuelno gasenje");
    GLCD_Rectangle(x1_vrednost, y1_vrednost, x2_vrednost, y2_vrednost);
    GoToXY(25,5);		
	GLCD_Printf ("Ugasi sistem");
        
    Zujalica(2,8,3);
    Delay_ms(300);
        
    while(currentState==STATE_PIR_ACTIVATED){
        Touch_Panel();
            if(X>x1_vrednost &&  X<x2_vrednost && Y>y1_vrednost-15 && Y<y2_vrednost-15){ 
                Delay_ms(30);
                currentState=STATE_DISARMED;
            }
    }
         
}

//==========================================MAIN=================================================================================================

int main (int argc, char** argv)
{    
init_main();
currentState = STATE_DISARMED;
previousState = STATE_DISARMED;

	while(1)
	{
        check_state();
        handle_state(); 
	}
return 0;
}