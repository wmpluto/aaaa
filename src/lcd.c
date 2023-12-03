/*****************************************************************************
*								lcd.c
*
* LCD Driver
* Configured specifically for the AAAA Clock demo LCD layout and connections
* Easily modified to be used with any other LCD layout and conenctions
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
* Michael Pearce	09/08/2010		Set up for AAAA LCD Clock Demo V2 hardware
******************************************************************************/
#include <pic.h>

#include "lcd.h"



/***********************************************************
*                    Character maps
*
* map_numeric = character map for numbers 0 to 9
*
* map_special = special characters that use the 7 segments
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
***********************************************************/
#define CHARMAPSIZE 22
unsigned char map_numbers[CHARMAPSIZE]={
								0b00111111,		// 0
								0b00000110,		// 1
								0b01101101,		// 2
								0b01001111,		// 3
								0b01010110,		// 4
								0b01011011,		// 5
								0b01111011,		// 6
								0b00001110,		// 7
								0b01111111,		// 8
								0b01011110,		// 9
								0b01100001,		// c
								0b01111000,		// F
								0b01111001,		// E
								0b01100000,		// r
								0b01110011,		// b
								0b01111110,		// A
								0b01110001,		// t
								0b00110001,		// L
								0b01100011,		// o
								0b01100010,		// n
								0b00000000,		// SPACE
								0b01111100}; 	// P
						
							



/******************************************************************************
* Function: void lcd_init (void)
*
* Overview: Initialise the LCD module using settings defined in lcd.h
*
* Input:    None
*
* Output:   None
*
******************************************************************************/
void lcd_init(void)
{
    
	LCDCON=LCDCON_LOAD;     // LCD Control Register - General Configuration
	LCDPS=LCDPS_LOAD;       // LCD Phase resister - Multiplexing/phse setup for LCD
	LCDREF=LCDREF_LOAD;     // LCD Reference ladder settings
	LCDCST=LCDCST_LOAD;     // LCD Contrast control register
	LCDRL=LCDRL_LOAD;		// LCD Reference Voltage Control Register

	LCDSE0=LCDSE0_LOAD; 	// LCD Segment Pin Selection Registers  
	LCDSE1=LCDSE1_LOAD;	
	//LCDSE2=LCDSE2_LOAD;	// Larger parts only

	lcd_clear();			// Clear the LCD memory

}

/******************************************************************************
* Function: void lcd_clear (void)
*
* Overview: Erases all data on the LCD
*			Alter for different part and displays used
*
* Input:    None
*
* Output:   None
*
******************************************************************************/
void lcd_clear(void)
{
	LCDDATA0=0x00;
	LCDDATA1=0x00;
//	LCDDATA2=0x00;
	LCDDATA3=0x00;
	LCDDATA4=0x00;
//	LCDDATA5=0x00;
	LCDDATA6=0x00;
	LCDDATA7=0x00;
	LCDDATA9=0x00;
	LCDDATA10=0x00;
//	LCDDATA11=0x00;
//	LCDDATA12=0x00;
//	LCDDATA13=0x00;
//	LCDDATA14=0x00;
//	LCDDATA15=0x00;
}


/******************************************************************************
* Function: void lcd_clear_special(void)
*
* Overview: Erases only the special characters
*
* Input:    None
*
* Output:   None
*
******************************************************************************/
void lcd_clear_special(void)
{

	SEG_BAT1=0;
	SEG_BAT2=0;
	SEG_BAT3=0;
	SEG_BAT4=0;

	SEG_COLON=0;
	SEG_MCHP=0;
	SEG_F1=0;
	SEG_F2=0;
	SEG_F3=0;
	SEG_F4=0;

}


/******************************************************************************
* Function: void lcd_putc(unsigned char num, unsigned char segment, unsigned char dot)
*
* Overview: Puts a number/character on the selected segment and the DOT if required.
*			For different displays you can add or remove segments as required.
*
* Input:    unsigned char num  -  points to the location in the array.
*                                 0 to 9 are numbers, above that special characters
*
*           unsigned char segment  -  The segment numebr to display the data on
*
*           unsigned char dot  -  If the lowest bit is 1 then the DOT is displayed
*
* Output:   None
*
******************************************************************************/
void lcd_putc(unsigned char num, unsigned char segment, unsigned char dot)
{
	unsigned char map;

	if(num >= CHARMAPSIZE) return;	// Dont display if it doesnt exist
	map=map_numbers[num];			// Get the dot map from the array
	dot &=0x01;						// Only interested in the lowest Bit for teh dot 
					

	switch(segment)
	{
		case 0:
			if(map & 1)SEG_A0=1; else SEG_A0=0;
			map>>=1;
			if(map & 1)SEG_A1=1; else SEG_A1=0;
			map>>=1;
			if(map & 1)SEG_A2=1; else SEG_A2=0;
			map>>=1;
			if(map & 1)SEG_A3=1; else SEG_A3=0;
			map>>=1;
			if(map & 1)SEG_A4=1; else SEG_A4=0;
			map>>=1;
			if(map & 1)SEG_A5=1; else SEG_A5=0;
			map>>=1;
			if(map & 1)SEG_A6=1; else SEG_A6=0;
			map>>=1;
			if(dot) SEG_ADOT=1; else SEG_ADOT=0;
			break;

		case 1:
			if(map & 1)SEG_B0=1; else SEG_B0=0;
			map>>=1;
			if(map & 1)SEG_B1=1; else SEG_B1=0;
			map>>=1;
			if(map & 1)SEG_B2=1; else SEG_B2=0;
			map>>=1;
			if(map & 1)SEG_B3=1; else SEG_B3=0;
			map>>=1;
			if(map & 1)SEG_B4=1; else SEG_B4=0;
			map>>=1;
			if(map & 1)SEG_B5=1; else SEG_B5=0;
			map>>=1;
			if(map & 1)SEG_B6=1; else SEG_B6=0;
			map>>=1;
			if(dot)SEG_BDOT=1; else SEG_BDOT=0;
			break;

		case 2:
			if(map & 1)SEG_C0=1; else SEG_C0=0;
			map>>=1;
			if(map & 1)SEG_C1=1; else SEG_C1=0;
			map>>=1;
			if(map & 1)SEG_C2=1; else SEG_C2=0;
			map>>=1;
			if(map & 1)SEG_C3=1; else SEG_C3=0;
			map>>=1;
			if(map & 1)SEG_C4=1; else SEG_C4=0;
			map>>=1;
			if(map & 1)SEG_C5=1; else SEG_C5=0;
			map>>=1;
			if(map & 1)SEG_C6=1; else SEG_C6=0;
			map>>=1;
			if(dot)SEG_CDOT=1; else SEG_CDOT=0;
		
			break;

		case 3:
			if(map & 1)SEG_D0=1; else SEG_D0=0;
			map>>=1;
			if(map & 1)SEG_D1=1; else SEG_D1=0;
			map>>=1;
			if(map & 1)SEG_D2=1; else SEG_D2=0;
			map>>=1;
			if(map & 1)SEG_D3=1; else SEG_D3=0;
			map>>=1;
			if(map & 1)SEG_D4=1; else SEG_D4=0;
			map>>=1;
			if(map & 1)SEG_D5=1; else SEG_D5=0;
			map>>=1;
			if(map & 1)SEG_D6=1; else SEG_D6=0;
			map>>=1;
			if(dot)SEG_DDOT=1; else SEG_DDOT=0;
		
			break;

		case 4:
			if(map & 1)SEG_E0=1; else SEG_E0=0;
			map>>=1;
			if(map & 1)SEG_E1=1; else SEG_E1=0;
			map>>=1;
			if(map & 1)SEG_E2=1; else SEG_E2=0;
			map>>=1;
			if(map & 1)SEG_E3=1; else SEG_E3=0;
			map>>=1;
			if(map & 1)SEG_E4=1; else SEG_E4=0;
			map>>=1;
			if(map & 1)SEG_E5=1; else SEG_E5=0;
			map>>=1;
			if(map & 1)SEG_E6=1; else SEG_E6=0;
			map>>=1;
			if(dot)SEG_EDOT=1; else SEG_EDOT=0;
		
			break;


	}//END switch(segment)

}


