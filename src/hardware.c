/*********************************************
*                 hardware.c
*********************************************/
#include <pic.h>
#include "hardware.h"

/*********************************************
*      Configuration Bit Settings
*********************************************/
//__CONFIG(FOSC_INTOSC&WDTE_OFF&PWRTE_ON&MCLRE_ON&CP_OFF&CPD_OFF&BOREN_OFF&CLKOUTEN_OFF&IESO_OFF&FCMEN_ON);
//__CONFIG(WRT_OFF&PLLEN_OFF&STVREN_OFF&BORV_19&LVP_OFF);


//*****************************************************************************
// PIC Configuration
//*****************************************************************************
__CONFIG( FOSC_INTOSC & WDTE_OFF & PWRTE_OFF & MCLRE_ON & CP_OFF & CPD_OFF & BOREN_OFF & CLKOUTEN_OFF & IESO_OFF & FCMEN_OFF);

__CONFIG(WRT_OFF & PLLEN_OFF & STVREN_OFF & BORV_25 & LVP_OFF);



/******************************************************************************
* Function: void Init (void)
*
* Overview: This function is used by the main program for general setup
*           Most of the original code is used, but there are a few modifications
*           for the CCP interrupt system and the ADC and variable configuration
*
* Input:    None
*
* Output:   None
*
******************************************************************************/
void hardware_init(void)
{
    // preset system variables
    block       = 0b00000000;               // reset system flags
    CS_statevar = 0;                        // reset statevariable for cap touch
    first       = 1;                        // preload average with initial values

    // Configure GPIO
    LATA    = LATA_LOAD;                    // this section cut from given code
    LATB    = LATB_LOAD;
    LATC    = LATC_LOAD;

    TRISA   = TRISA_LOAD;
    TRISB   = TRISB_LOAD;
    TRISC   = TRISC_LOAD;

    ANSELA  = ANSELA_LOAD;
    ANSELB  = ANSELB_LOAD;

    // Timer1 configuration
    TMR1L   = 0b00000000;                   // clear timer
    TMR1H   = 0b00000000;
    T1CON   = 0b10001101;                   // T1OSC, no pscale, OSC on, sync on, TMR1 on
    T1GCON  = 0b00000000;                   // disable TMR1 gate

    // ADC configuration for cap touch
    ADCON0   = ADC_BTN2;                    // startup with button 2 selected AN10
    ADCON1   = 0b10110000;                  // rt just, ADC osc, VDD and VSS for Vref

    // Start cap sense with first button
    BTN1_out;                               // charge ADC Chold
    BTN1_high;
    ADCON0   = ADC_BTN1;
    GO_nDONE = 1;                           // disconnect ADC Chold
    BTN1_low;                               // discharge sensor BTN 1
    BTN1_in;                                // disconnect output driver
    GO_nDONE = 0;                           // reconnect ADC Chold to sensor
    GO_nDONE = 1;                           // start conversion of value

    // CCP2 configuration for 1 second timeout on TMR1 with 32 kHz crystal
    CCP2CON = 0b00001010;                   // configure CCP2 for compare, INT only
    CCPR2L  = 0x00;
    CCPR2H  = 0x10;                         // configure for first of 8 interrupts / second

    // Configure system interrups
    CCP2IF  = 0;                            // clear any pending interrupts
    CCP2IE  = 1;                            // enable peripheral's ability to interrupt

    PEIE    = 1;                            // enable peripheral interrupts
    GIE     = 1;                            // enable system interrupts
}


#if(0) // ORIGINAL CODE BELOW
/*********************************************
*             hardware_init
*********************************************/
void hardware_init(void)
{
	// Set up Timer1 OSC to run system and generate a 1 second tick for the clock
	// Do before changing OSCCON to give it a little chance to start up before change over
#ifdef CLOCK_EXTERNAL
	T1CON=T1OSCSTART;
    while(T1OSCR==0);  // Wait for Timer 1 oscillator to start up and run
#endif

	TMR1L=TMR1L_LOAD;
	TMR1H=TMR1H_LOAD;
	T1CON=T1CON_LOAD;
	T1GCON=T1GCON_LOAD;


	// I/O pin output settings
	LATA=LATA_LOAD;
	LATB=LATB_LOAD;
	LATC=LATC_LOAD;

	// TRIS settings
	TRISA=TRISA_LOAD;
	TRISB=TRISB_LOAD;
	TRISC=TRISC_LOAD;

	// ADC settings
	ADCON0=ADCON0_LOAD;
	ADCON1=ADCON1_LOAD;
	ANSELA=ANSELA_LOAD;
	ANSELB=ANSELB_LOAD;

	// Internal Reference Setup
	FVRCON=FVRCON_LOAD;

	// Set Up PWM
#ifdef USE_PWM
	PR2=PR2_LOAD;
	T2CON=T2CON_LOAD;
	PSTR1CON=PSTR1CON_LOAD;
	CCP1CON=CCP1CON_LOAD;
	CCPR1L=CCPR1L_LOAD;
#endif

	// Set up Oscillator to use the external 32.768 Oscillator
	OSCCON=OSCCON_LOAD;
   

}
#endif  // End Original Code

/**** END OF hardware.c ******/


