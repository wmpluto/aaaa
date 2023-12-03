/*****************************************************************************
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
* Author                Date        Comment
*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
* Michael Pearce		10/08/2010  Add the ALARM define 
* Michael Pearce		09/08/2010	Set up for AAAA LCD Clock Demo V2
* Keith Curtis          08/19/2010  Simple touch code for demo
******************************************************************************/
#ifndef _MAIN_H
#define _MAIN_H

//*****************************************************************************
// Include and Header files
//*****************************************************************************
#include "pic.h"
#include "GenericTypeDefs.h"

//*****************************************************************************
// Global Knobs
//*****************************************************************************
//#define USE_ALARM				// COMMENT OUT TO DISABLE THE ALARM FEATURES


//*****************************************************************************
// Global Definitions and Equates
//*****************************************************************************

#define ADC_Batt        0b00000001              // select AN0 battery sense
#define ADC_Temp        0b00100101              // select AN9 temperature

#define ADC_BTN2        0b00101001              // select AN10 Button 2
#define ADC_BTN1        0b00110001              // select AN12 Button 1

#define threshold       20                      // Power up default threshold for valid press
#define thresholdmin	15						// Min threshold allowed
#define thresholdmax	35						// Max threshold allowed
#define AVGRST_MAX		60*4					// If key held for more than 60 seconds reset it

//**** cap touch defines for CVD ****
#define BTN1_in         TRISB0  = 1             // make RB0 an input
#define BTN1_out        TRISB0  = 0             // make RB0 an output
#define BTN1_low        LATB0   = 0             // set RB0 low
#define BTN1_high       LATB0   = 1             // set RB0 high

#define BTN2_in         TRISB1  = 1             // make RB1 an input
#define BTN2_out        TRISB1  = 0             // make RB1 an output
#define BTN2_low        LATB1   = 0             // set RB1 low
#define BTN2_high       LATB1   = 1             // set RB1 high



//**** Set up default PORT and TRIS configurations for the pins ****
#define LATA_LOAD       0b11111111
#define TRISA_LOAD      0b11111111

#define LATB_LOAD       0b11111000              // RB0-1 low for CVD
#define TRISB_LOAD      0b11111000              // RB0-1 outputs for sense dischrg

#define LATC_LOAD       0b10111111
#define TRISC_LOAD      0b10111111

#define ANSELA_LOAD     0b00000001              // RA0 is Analog In (Bat Volts)
#define ANSELB_LOAD     0b00001011              // RB3 temp, RB0-1 cap touch



#endif
