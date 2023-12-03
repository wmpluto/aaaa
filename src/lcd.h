/*****************************************************************************
*								lcd.h
*
* LCD Driver for AAAA LCD Clock demo V2 Hardware
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
#ifndef _LCD_H
#define _LCD_H

#include "hardware.h"

/*********************************************
* Global Functions
*********************************************/
extern void lcd_init(void);
extern void lcd_clear(void);
extern void lcd_clear_special(void);
extern void lcd_putc(unsigned char num, char segment, char dot);




/*********************************************
* Defines to set up the LCD Module
*********************************************/
#define LCD_CLOCK_EXTERNAL				// Use the external 32.768KHz Oscillator

#ifdef LCD_CLOCK_EXTERNAL					
 #define LCDCON_LOAD	0b10000111		// T1OSC - 32.768KHz external
#else
 #define LCDCON_LOAD	0b10001011		// Internal Oscillator
#endif


#define LCDPS_LOAD		0b00000001
#define LCDREF_LOAD		0b10000000
#define LCDCST_LOAD		0b00000001
//#define LCDRL_LOAD		0b01010001	// LCD REF - Low power mode
#define LCDRL_LOAD		0b10010001		// LCD REF - Med Power mode (Better contrast)

#define LCDSE0_LOAD		0b11111110
#define LCDSE1_LOAD		0b01101101





/************************************************************
* 7 Segment to Ram location mapping for Multimeter LCD
*	 D C B A   - Character Number	
*    1.8.8.8   - Character position
*
* Change this to match your specific configuration
*
* Bit numbers relating to segments are shown below
*
*   		333
*          4   2
*          4   2
*           666
*          5   1
*          5   1
*           000   7  
*
************************************************************/
#define SEG_DEAD	SEG15COM3 // Used for Dead segments.

#define SEG_A0		SEG8COM1 
#define SEG_A1		SEG10COM0
#define SEG_A2		SEG10COM3
#define SEG_A3		SEG8COM3
#define SEG_A4		SEG8COM2
#define SEG_A5		SEG8COM0
#define SEG_A6		SEG10COM2
#define SEG_ADOT	SEG_DEAD  // No Dot so use dead segment

#define SEG_B0		SEG2COM1
#define SEG_B1		SEG1COM0
#define SEG_B2		SEG1COM3
#define SEG_B3		SEG2COM3
#define SEG_B4		SEG2COM2	
#define SEG_B5		SEG2COM0
#define SEG_B6		SEG1COM2
#define SEG_BDOT	SEG1COM1

#define SEG_C0		SEG7COM1
#define SEG_C1		SEG5COM0
#define SEG_C2		SEG5COM3
#define SEG_C3		SEG7COM3
#define SEG_C4		SEG7COM2
#define SEG_C5		SEG7COM0
#define SEG_C6		SEG5COM2
#define SEG_CDOT	SEG5COM1

#define SEG_D0		SEG13COM1
#define SEG_D1		SEG14COM0
#define SEG_D2		SEG14COM3
#define SEG_D3		SEG13COM3
#define SEG_D4		SEG13COM2
#define SEG_D5		SEG13COM0
#define SEG_D6		SEG14COM2
#define SEG_DDOT	SEG14COM1


#define SEG_E0		SEG6COM3
#define SEG_E1		SEG11COM2
#define SEG_E2		SEG11COM1
#define SEG_E3		SEG6COM1
#define SEG_E4		SEG6COM0
#define SEG_E5		SEG6COM2
#define SEG_E6		SEG11COM0
#define SEG_EDOT	SEG_DEAD	// No Dot on this one

/*********************************************
*  Special individual segments on the display
**********************************************/
#define SEG_BAT1	SEG4COM3
#define SEG_BAT2	SEG4COM0
#define SEG_BAT3	SEG4COM2
#define SEG_BAT4	SEG4COM1

#define SEG_COLON	SEG10COM1
#define SEG_MCHP	SEG3COM1
#define SEG_F1		SEG3COM3
#define SEG_F2		SEG11COM3
#define SEG_F3		SEG3COM2
#define SEG_F4		SEG3COM0

#define SEG_MINUS	SEG14COM2	// Part of the 4th digit

/*************************************************
* Position of additional characters in the array.
* This array is in lcd.c and can be customized
* to suit your application.
*************************************************/
#define CHAR_c		10
#define CHAR_F		11
#define CHAR_E		12
#define CHAR_r		13
#define CHAR_b		14
#define CHAR_A		15
#define CHAR_t		16
#define	CHAR_L		17
#define CHAR_o		18
#define CHAR_n		19
#define CHAR_SPACE  20
#define CHAR_P		21

#endif


