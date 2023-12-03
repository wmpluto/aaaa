/*****************************************************************************
*								hardware.h
*
* AAAA LCD Clock demo V2 Hardware
*
*
* CODE OWNERSHIP AND DISCLAIMER OF LIABILITY
*
* Microchip Technology Incorporated ("Microchip") retains all ownership and
* intellectual property rights in the code accompanying this message and in
* all derivatives hereto.  You may use this code, and any derivatives created
* by any person or entity by or on your behalf, exclusively with Microchip’s
* proprietary products.  Your acceptance and/or use of this code constitutes
* agreement to the terms and conditions of this notice.
*
* CODE ACCOMPANYING THIS MESSAGE IS SUPPLIED BY MICROCHIP "AS IS".  NO
* WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT NOT
* LIMITED TO, IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY AND
* FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS CODE, ITS INTERACTION WITH
* MICROCHIP’S PRODUCTS, COMBINATION WITH ANY OTHER PRODUCTS, OR USE IN ANY
* APPLICATION.
*
* YOU ACKNOWLEDGE AND AGREE THAT, IN NO EVENT, SHALL MICROCHIP BE LIABLE,
* WHETHER IN CONTRACT, WARRANTY, TORT (INCLUDING NEGLIGENCE OR BREACH OF
* STATUTORY DUTY), STRICT LIABILITY, INDEMNITY, CONTRIBUTION, OR OTHERWISE,
* FOR ANY INDIRECT, SPECIAL, PUNITIVE, EXEMPLARY, INCIDENTAL OR CONSEQUENTIAL
* LOSS, DAMAGE, FOR COST OR EXPENSE OF ANY KIND WHATSOEVER RELATED TO THE
* CODE, HOWSOEVER CAUSED, EVEN IF MICROCHIP HAS BEEN ADVISED OF THE
* POSSIBILITY OR THE DAMAGES ARE FORESEEABLE.  TO THE FULLEST EXTENT ALLOWABLE
* BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN ANY WAY RELATED TO THIS
* CODE, SHALL NOT EXCEED THE PRICE YOU PAID DIRECTLY TO MICROCHIP SPECIFICALLY
* TO HAVE THIS CODE DEVELOPED.
*
* You agree that you are solely responsible for testing the code and
* determining its suitability.  Microchip has no obligation to modify, test,
* certify, or support the code.
*
* Author            Date			Comment
*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
* Michael Pearce	09/08/2010		Set up for AAAA LCD Clock Demo V2
******************************************************************************/
#ifndef _HARDWARE_H
#define _HARDWARE_H
/*********************************************
*                 Hardware.h
*********************************************/

#include "GenericTypeDefs.h"

#define USE_PWM					// Use PWM to output to speaker

#define CLOCK_EXTERNAL			// Use External 32.768KHz crystal

#define MCP9701					// Use MCP9701 Temperature sensor
//#define MCP9700

#define DEGREES_C				// Start in Degrees C
//#define DEGREES_F

#if defined (MCP9700)			// Select appropiate Offset and Divisor for temperature sensor used
 #define T_OFFSET_ZERO  500
 #define T_DIVISOR		100
#elif defined (MCP9701)
 #define T_OFFSET_ZERO	400				
 #define T_DIVISOR		195
#else
 #error "Must be MCP9700 or MCP9701"
#endif

#define BAT_LEVEL_MAX	1300  	// Set the leves for the battery level warnings
#define BAT_LEVEL_MED	1150  	// Anything above MAX show all bars, above MED shows
#define BAT_LEVEL_MIN	1000  	// 2 bars, above MIN 1 bar, below min no bars

#define SPEAKER		LATB1		// Use PWM on P1C to drive the speaker 
#define TEMP_EN		LATB2		// Used to turn sensor on and off to save power
#define TEMP_IN		RB3			// AN9 used to read sensor
#define BATV_IN		RA0			// AN0 used for measuring voltage on battery

#define SW1 BTN1
#define SW2 BTN2


// Set up default PORT and TRIS configurations fo the pins

#define LATA_LOAD       0b11111111
#define TRISA_LOAD      0b11111111

#define LATB_LOAD       0b11111000  // RB0-1 low for CVD
#define TRISB_LOAD      0b11111000  // RB0-1 outputs for sense dischrg

#define LATC_LOAD       0b10111111
#define TRISC_LOAD      0b10111111

// The remaining pins are segment drivers and set up in the LCD files.


// Analog Configuration
#define ADCON0_LOAD 0b00000000		// Leave ADC powered off to save power
#define ADCON1_LOAD_CAPS 0b10110000
#define ADCON1_LOAD_2048 0b10110011

#define ANSELA_LOAD     0b00000001  // RA0 is Analog In (Bat Volts)
#define ANSELB_LOAD     0b00001011  // RB3 temp, RB0-1 cap touch


// Internal VREF Configuration
#define FVRCON_LOAD 0b00000010      // Disabled, but configured for 2.048V to ADC only

// PWM Configuration
#define CCP3CON_LOAD	0b00001100  // PWM mode, output steering LSBs=0
#define PSTR3CON_LOAD	0b00000001  // Steer output to P3A
#define PR2_LOAD		16			// 
#define T2CON_LOAD		0b00000100  // Pre & Post 1:1 and turn ON
#define CCPR3L_LOAD		0			// Load zero to start with so no output

// Timer 1 configuration
#ifdef CLOCK_EXTERNAL
 #define T1OSCSTART  0b10001100		// Start oscillator Before starting timer
 #define T1CON_LOAD  0b10001101		// Use T1OSC, 1:1, OSCEN, T1ON
 #define T1GCON_LOAD 0b00000000		// Do not use gating
 #define TMR1H_LOAD	 0xF0			// 32768 / 8 = 4096  
 #define TMR1L_LOAD	 0x00			// 0xFFFF - 4096 + 1 = 0xF000

 #define OSCCON_LOAD 0b00111010		// Internal Oscillator 500KHz

#else
	#error "MUST SELECT EXTERNAL OSCILLATOR OPTION"
#endif


// Masks for ADC selection
#define ADC_SEL_TEMPERATURE 0b00100101	// AN9 is Temperature sensor
#define ADC_SEL_BATTERY		0b00000001	// AN0 is battery monitor


//extern void hardware_init(void);


/**** END OF hardware.h ******/
#endif
