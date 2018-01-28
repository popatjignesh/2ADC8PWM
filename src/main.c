/* 
 * File:   main.c
 * Author: Jignesh D. Popat
 *
 * Created on January 7, 2018, 6:54 PM
 */

#include "xc.h"
#include <stdio.h>
#include <stdlib.h>

#define _XTAL_FREQ 8000000

#define MAX_ADC 1024

#define SW_MAIN     PORTAbits.RA0       //Main Switch
#define AN1         PORTAbits.RA1       //ADC Input PIN 1
#define AN2         PORTAbits.RA2       //ADC Input PIN 2
#define AN_UV       PORTAbits.RA3       //ADC Input PIN 3
#define AN_OT       PORTAbits.RA4       //ADC Input PIN 4
#define AN_OC     	PORTAbits.RA5		//ADC Input PIN 5
#define SW_FW       PORTCbits.RC0       //SW_FW Input PIN
#define SW_RW       PORTCbits.RC1       //SW_RW Input PIN
#define SW_DR       PORTCbits.RC2       //SW_DR Input PIN
#define SW_DL       PORTCbits.RC3       //SW_DL Input PIN
#define SW_INC      PORTCbits.RC4       //SW_INC Input PIN
#define SW_DIC      PORTCbits.RC5       //SW_DIC Input PIN
#define SW_HN       PORTBbits.RB6       //SW_HN Input PIN

#define OUT_R1      LATAbits.LATA6      //OUT_R1 Output PIN
#define OUT_R2      LATAbits.LATA7      //OUT_R2 Output PIN
#define OUT_HN      LATBbits.LATB7      //OUT_HN Output PIN
#define LED_UV      LATEbits.LATE0      //LED_UV Output PIN
#define LED_OT      LATEbits.LATE1      //LED_OT Output PIN
#define LED_OC      LATEbits.LATE2      //LED_OC Output PIN
#define LED_20      LATDbits.LATD3      //LED_20 Output PIN
#define LED_40      LATDbits.LATD0      //LED_40 Output PIN
#define LED_60      LATDbits.LATD1      //LED_60 Output PIN
#define LED_80      LATDbits.LATD4      //LED_80 Output PIN
#define LED_100     LATDbits.LATD5      //LED_100 Output PIN
#define MCU_LED     LATDbits.LATD2      //MCU_LED Output PIN

/*
#define PWM0        LATBbits.LATB0      //PWM0 Output PIN
#define PWM1        LATBbits.LATB1      //PWM1 Output PIN
#define PWM2        LATBbits.LATB2      //PWM2 Output PIN
#define PWM3        LATBbits.LATB3      //PWM3 Output PIN
#define PWM4        LATBbits.LATB5      //PWM4 Output PIN
#define PWM5        LATBbits.LATB4      //PWM5 Output PIN
#define PWM6        LATDbits.LATD6      //PWM6 Output PIN
#define PWM7        LATDbits.LATD7      //PWM7 Output PIN
*/

void ConfigCLK(void);
void ConfigIO(void);
void ConfigPWM(void);

void InitADC1(void);
void InitADC2(void);

void ConfigDutyCycle1(void);
void ConfigDutyCycle2(void);
void DisableAllPWM(void);
void FirePWM0347(void);
void FirePWM1256(void);
void FirePWM0356(void);
void FirePWM1247(void);

void CheckHN(void);
void CheckVOLT(void);
void CheckCURRENT(void);
void CheckTEMPERATURE(void);
void CheckIncDic(void);
void CheckMode(void);

//void delay_ms(unsigned int count);

unsigned char temp8;
unsigned char count;
unsigned char PWMH, PWML;

unsigned int temp16;
unsigned int PWM16 = 0;
unsigned int PreviousADC1, ADC1result, PreviousADC2, ADC2result;
int ADC1difference, ADC2difference;
unsigned int MaxDC;

void main()
{
    ConfigCLK();
    NOP();
    ConfigIO();
    NOP();
    ConfigPWM();
    NOP();
    
    count = 0;
    
    while(1)
    {
		CheckVOLT();
		CheckCURRENT();
		CheckTEMPERATURE();
		
    	if(SW_MAIN)
    	{
            OUT_R1 = OUT_R2 = 1;
            MCU_LED = 1;
            
            CheckHN();
            CheckIncDic();
            CheckMode();
	    }
        else
        {
            OUT_R1 = OUT_R2 = 0;
            MCU_LED = 0;
        }
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

void ConfigIO(void)
{
	ANSEL0 = 0x00;          //Set all pins as Digital
	ANSEL1 = 0x00;

	TRISA = 0;              //Set all pins as Output
	TRISB = 0;
	TRISC = 0;
	TRISD = 0;
	TRISE = 0;

	TRISAbits.TRISA0 = 1;	//Main Switch
    TRISAbits.TRISA1 = 1;   //ADC as an input 1
    TRISAbits.TRISA2 = 1;   //ADC as an input 2
	TRISAbits.TRISA3 = 1;   //ADC as an input 3
	TRISAbits.TRISA4 = 1;   //ADC as an input 4
	TRISAbits.TRISA5 = 1;	//ADC as an input 5
	TRISCbits.TRISC0 = 1;	//SW_FW Input PIN
	TRISCbits.TRISC1 = 1;	//SW_RW Input PIN
	TRISCbits.TRISC2 = 1;	//SW_DR Input PIN
	TRISCbits.TRISC3 = 1;	//SW_DL Input PIN
	TRISCbits.TRISC4 = 1;	//SW_INC Input PIN
	TRISCbits.TRISC5 = 1;	//SW_DIC Input PIN
	TRISBbits.TRISB6 = 1;	//SW_HN Input PIN

	TRISAbits.TRISA6 = 0;	//OUT_R1 Output PIN
	TRISAbits.TRISA7 = 0;   //OUT_R2 Output PIN
	TRISBbits.TRISB7 = 0;   //OUT_HN Output PIN
	TRISBbits.TRISB0 = 0;   //PWM0 as an output
	TRISBbits.TRISB1 = 0;   //PWM1 as an output
	TRISBbits.TRISB2 = 0;   //PWM2 as an output
	TRISBbits.TRISB3 = 0;   //PWM3 as an output
	TRISBbits.TRISB5 = 0;   //PWM4 as an output
	TRISBbits.TRISB4 = 0;   //PWM5 as an output
	TRISDbits.TRISD6 = 0;   //PWM6 as an output
	TRISDbits.TRISD7 = 0;   //PWM7 as an output
	TRISEbits.TRISE0 = 0;	//LED_UV Output PIN
	TRISEbits.TRISE1 = 0;	//LED_OT Output PIN
	TRISEbits.TRISE2 = 0;	//LED_OV Output PIN
	TRISDbits.TRISD3 = 0;	//LED_20 Output PIN
	TRISDbits.TRISD0 = 0;	//LED_40 Output PIN
	TRISDbits.TRISD1 = 0;	//LED_60 Output PIN
	TRISDbits.TRISD4 = 0;	//LED_80 Output PIN
	TRISDbits.TRISD5 = 0;	//LED_100 Output PIN
	TRISDbits.TRISD2 = 0;	//MCU_LED Output PIN

    LATA = 0;               //all pins output 0 value
	LATB = 0;
	LATC = 0;
	LATD = 0;
	LATE = 0;
}

void ConfigPWM(void)
{
    PTCON0bits.PTOPS = 0b0000;      //1:1 Postscale
    PTCON0bits.PTCKPS = 0b00;       //Fosc/4 (1:1)
    PTCON0bits.PTMOD = 0b00;        //Free running mode

    PTCON1bits.PTEN = 1;            //time base ON
    PTCON1bits.PTDIR = 0;           //time base counts up
    
    PWMCON0bits.PMOD = 0b1111;      //all PWM are independent

    PTPERL = 0x65;                  //Generate PWM with 19.61 KHz
    PTPERH = 0x00;
}

void InitADC1(void)
{
    TRISAbits.TRISA1 = 1;       //ADC AN1 Input PIN Enabled
    ANSEL0bits.ANS1 = 1;        //Configure RA1 as an Analog channel AN1
    ADCON1bits.VCFG = 0b00;     //AVdd as +Vref and AVss as -Vref
    ADCHSbits.GBSEL0 = 0;       //Group B - AN1
    ADCHSbits.GBSEL1 = 0;
    ADCON0bits.ACONV = 0;       //Single-shot mode enabled
    ADCON0bits.ACSCH = 0;       //Single channel mode selected
    ADCON0bits.ACMOD = 0b01;    //Group B is taken and converted
    ADCON2bits.ADCS = 0b001;    //Fosc/8
    ADCON2bits.ACQT = 0b0001;   //2Tad
    ADCON0bits.ADON = 1;        //ADC is enabled
    __delay_ms(10);             //give 10ms delay as given in datasheet
    ADCON0bits.GO = 1;          //Start ADC
    while(ADCON0bits.GO);		//Wait till ADC Conversation complete
	ADC1result = ADRES;         //Get ADC Result
}

void InitADC2(void)
{
    TRISAbits.TRISA2 = 1;       //ADC AN2 Input PIN Enabled
    ANSEL0bits.ANS2 = 1;        //Configure RA1 as an Analog channel AN2
    ADCON1bits.VCFG = 0b00;     //AVdd as +Vref and AVss as -Vref
    ADCHSbits.GCSEL0 = 0;       //Group C - AN2
    ADCHSbits.GCSEL1 = 0;
    ADCON0bits.ACONV = 0;       //Single-shot mode enabled
    ADCON0bits.ACSCH = 0;       //Single channel mode selected
    ADCON0bits.ACMOD = 0b10;    //Group B is taken and converted
    ADCON2bits.ADCS = 0b001;    //Fosc/8
    ADCON2bits.ACQT = 0b0001;   //2Tad
    ADCON0bits.ADON = 1;        //ADC is enabled
    __delay_ms(10);             //give 10ms delay as given in datasheet
    ADCON0bits.GO = 1;          //Start ADC
    while(ADCON0bits.GO);		//Wait till ADC Conversation complete
	ADC2result = ADRES;         //Get ADC Result
}

void ConfigDutyCycle1(void)
{
    PWM16 = (unsigned int)((ADC1result * MaxDC)/MAX_ADC);

    temp16 = PWM16;
    temp16 = temp16 & 0xFF00;
    temp16 = temp16>>8;
    temp16 = temp16 & 0x00FF;
    PWMH = (unsigned char)temp16;

    temp16 = PWM16;
    temp16 = temp16 & 0x00FF;
    PWML = (unsigned char)temp16;
}

void ConfigDutyCycle2(void)
{
    PWM16 = (unsigned int)((ADC2result * MaxDC)/MAX_ADC);
    
    temp16 = PWM16;
    temp16 = temp16 & 0xFF00;
    temp16 = temp16>>8;
    temp16 = temp16 & 0x00FF;
    PWMH = (unsigned char)temp16;

    temp16 = PWM16;
    temp16 = temp16 & 0x00FF;
    PWML = (unsigned char)temp16;
}

void DisableAllPWM(void)
{
    TRISBbits.TRISB0 = 1;           //Disable all output pins
	TRISBbits.TRISB1 = 1;
	TRISBbits.TRISB2 = 1;
	TRISBbits.TRISB3 = 1;
	TRISBbits.TRISB5 = 1;
	TRISBbits.TRISB4 = 1;
	TRISDbits.TRISD6 = 1;
	TRISDbits.TRISD7 = 1;
    
    PWMCON0bits.PWMEN = 0b000;      //Disable all PWM output
}

void FirePWM0347(void)
{
    PDC0L = PDC1L = PDC2L = PDC3L = PWML;   //Load Duty Cycle according to
    PDC0H = PDC1H = PDC2H = PDC3H = PWMH;   //respective ADC

    PIR3bits.PTIF = 0;              //Clear PWM Time Base Interrupt Flag
    PIE3bits.PTIE = 1;              //PWM Time Base Interrupt Enable bit
    while(PIR3bits.PTIF == 0);      ///Wait till PWM Time Base Interrupt Flag sets
    
    TRISBbits.TRISB0 = 0;           //Enable PWM0n3 output
	TRISBbits.TRISB3 = 0;
    TRISBbits.TRISB5 = 0;           //Enable PWM4n7 output
	TRISDbits.TRISD7 = 0;
    
    PWMCON0bits.PWMEN = 0b101;      //all PWM enabled
}

void FirePWM1256(void)
{
    PDC0L = PDC1L = PDC2L = PDC3L = PWML;   //Load Duty Cycle according to
    PDC0H = PDC1H = PDC2H = PDC3H = PWMH;   //respective ADC

    PIR3bits.PTIF = 0;              //Clear PWM Time Base Interrupt Flag
    PIE3bits.PTIE = 1;              //PWM Time Base Interrupt Enable bit
    while(PIR3bits.PTIF == 0);      ///Wait till PWM Time Base Interrupt Flag sets

    TRISBbits.TRISB1 = 0;           //Enable PWM1n2 output
	TRISBbits.TRISB2 = 0;
    TRISBbits.TRISB4 = 0;           //Enable PWM5n6 output
	TRISDbits.TRISD6 = 0;
    
    PWMCON0bits.PWMEN = 0b101;      //all PWM enabled
}

void FirePWM0356(void)
{
    PDC0L = PDC1L = PDC2L = PDC3L = PWML;   //Load Duty Cycle according to
    PDC0H = PDC1H = PDC2H = PDC3H = PWMH;   //respective ADC

    PIR3bits.PTIF = 0;              //Clear PWM Time Base Interrupt Flag
    PIE3bits.PTIE = 1;              //PWM Time Base Interrupt Enable bit
    while(PIR3bits.PTIF == 0);      ///Wait till PWM Time Base Interrupt Flag sets
    
    TRISBbits.TRISB0 = 0;           //Enable PWM0n3 output
	TRISBbits.TRISB3 = 0;
    TRISBbits.TRISB4 = 0;           //Enable PWM5n6 output
	TRISDbits.TRISD6 = 0;
    
    PWMCON0bits.PWMEN = 0b101;      //all PWM enabled
}

void FirePWM1247(void)
{
    PDC0L = PDC1L = PDC2L = PDC3L = PWML;   //Load Duty Cycle according to
    PDC0H = PDC1H = PDC2H = PDC3H = PWMH;   //respective ADC

    PIR3bits.PTIF = 0;              //Clear PWM Time Base Interrupt Flag
    PIE3bits.PTIE = 1;              //PWM Time Base Interrupt Enable bit
    while(PIR3bits.PTIF == 0);      ///Wait till PWM Time Base Interrupt Flag sets

    TRISBbits.TRISB1 = 0;           //Enable PWM1n2 output
	TRISBbits.TRISB2 = 0;
    TRISBbits.TRISB5 = 0;           //Enable PWM4n7 output
	TRISDbits.TRISD7 = 0;

    PWMCON0bits.PWMEN = 0b101;      //all PWM enabled
}

void CheckHN(void)
{
    if(SW_HN)
        OUT_HN = 1;
    else
        OUT_HN = 0;
}

void CheckVOLT(void)
{

}

void CheckCURRENT(void)
{

}

void CheckTEMPERATURE(void)
{

}

void CheckIncDic(void)
{
    if(SW_INC)
    {
        while(SW_INC);
        if(count >= 0 && count < 5)
            count = count+1;
    }

    if(SW_DIC)
    {
        while(SW_DIC);
        if(count > 0 && count <= 5)
            count = count-1;
    }

    if(count == 1)
    {
        LED_20 = 1;
        LED_40 = LED_60 = LED_80 = LED_100 = 0;
        MaxDC = 3276;       //Max 20% Duty Cycle
    }
    else if (count == 2)
    {
        LED_20 = LED_40 = 1;
        LED_60 = LED_80 = LED_100 = 0;
        MaxDC = 6552;       //Max 40% Duty Cycle
    }
    else if (count == 3)
    {
        LED_20 = LED_40 = LED_60 = 1;
        LED_80 = LED_100 = 0;
        MaxDC = 9828;       //Max 60% Duty Cycle
    }
    else if (count == 4)
    {
        LED_20 = LED_40 = LED_60 = LED_80 = 1;
        LED_100 = 0;
        MaxDC = 13104;       //Max 80% Duty Cycle
    }
    else if (count == 5)
    {
        LED_20 = LED_40 = LED_60 = LED_80 = LED_100 = 1;
        MaxDC = 16383;       //Max 100% Duty Cycle
    }
    else
    {
        LED_20 = LED_40 = LED_60 = LED_80 = LED_100 = 0;
        MaxDC = 0;       //Max 0% Duty Cycle
    }
}

void CheckMode(void)
{
    if(SW_FW)
    {
        InitADC1();
        ADC1difference = abs(ADC1result - PreviousADC1);
        if(ADC1difference > 9)
        {
            PreviousADC1 = ADC1result;
            ConfigDutyCycle1();
            DisableAllPWM();
            FirePWM0347();
        }
    }

    if(SW_RW)
    {
        InitADC1();
        ADC1difference = abs(ADC1result - PreviousADC1);
        if(ADC1difference > 9)
        {
            PreviousADC1 = ADC1result;
            ConfigDutyCycle1();
            DisableAllPWM();
            FirePWM1256();
        }
    }

    if(SW_DR)
    {
        InitADC2();
        ADC2difference = abs(ADC2result - PreviousADC2);
        if(ADC2difference > 9)
        {
            PreviousADC2 = ADC2result;
            ConfigDutyCycle2();
            DisableAllPWM();
            FirePWM0356();
        }
    }

    if(SW_DL)
    {
        InitADC2();
        ADC2difference = abs(ADC2result - PreviousADC2);
        if(ADC2difference > 9)
        {
            PreviousADC2 = ADC2result;
            ConfigDutyCycle2();
            DisableAllPWM();
            FirePWM1247();
        }
    }
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
