/* 
 * File:   LedBlink.c
 * Author: Jignesh D. Popat
 *
 * Created on January 7, 2018, 5:27 PM
 */

#include "xc.h"
#include <stdio.h>
#include <stdlib.h>

#define _XTAL_FREQ 8000000

#define MCU_LED		LATDbits.LATD2      //MCU_LED Output PIN

void ConfigCLK(void);
void InitIO(void);
//void delay_ms(unsigned int count);


void main()
{
    ConfigCLK();
    NOP();
    InitIO();
    NOP();
	
	MCU_LED = 0;
	
	while(1)
    {
		MCU_LED = ~MCU_LED;
		__delay_ms(1000);
	}
}

void ConfigCLK(void)
{
    OSCTUNE = 0x00;             //Center Frequency
    NOP();
    OSCCONbits.IRCF = 0b111;    //Internal Oscillator Frequency as 8 Mhz
    NOP();
    OSCCONbits.SCS = 0b11;      //Internal System Clock Select bits
    NOP();
}

void InitIO(void)
{
	ANSEL0 = 0x00;          //Set all pins as Digital
	ANSEL1 = 0x00;

	TRISA = 0;              //Set all pins as Output
	TRISB = 0;
	TRISC = 0;
	TRISD = 0;
	TRISE = 0;

    TRISDbits.TRISD2 = 0;   //Define MCU_LED as an output
    
    LATA = 0;               //all pins output 0 value
	LATB = 0;
	LATC = 0;
	LATD = 0;
	LATE = 0;
}

/*
void delay_ms(unsigned int count)
{
    for(unsigned int i = 0; i < count; i++)
    {
        for(unsigned int j = 0; j < 2000; j++);
    }
}
*/
