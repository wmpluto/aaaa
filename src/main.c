/*****************************************************************************
*								main.c
*
* AAAA LCD Clock demo.
*
* Features:
*		Time display in AM/PM or 24Hour formats
*
*		CVD cap sense for MODE and SET buttons
*
*		Temperature sensor read and is displayed in Degrees C or F
*
*		Battery voltage monitoring and display
*
*		Boost from single AAAA battery to 3.3V
*
*		Programmable Audible Alarm External Piezo needed
*		Introduced in V1.05 - #define USE_ALARM in main.h to enable
*
*		Cap Sense Calibration. By touching both buttons at same time, waiting for 
*		the F1 symbol, then releasing both buttons it will calibrate the cap sense.
*		Introduced in V1.06 to fix differing threshold between boards. 
*		Note: If you calibrate while holding the board then you may need to hold the
*		board for the keys to work. Removing the battery will reset the calibration
*		to the factory default value.
*
*
* Compiler Used: 
*		HITECH PICC V9.80
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
* Author            Date		Ver		Comment
*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
* Michael Pearce	10/26/2010	1.06	Added Cap sense Calibration.
*										Added Dual key lock detection (>40sec)
*										Added individual key lock detection (>60sec)
*										When in setup clear seconds only when setting hours or minutes
* Michael Pearce	10/07/2010	1.05	Add Alarm Option (#define in main.h to enable/disable)
* Michael Pearce	09/13/2010	1.04	Changed Bat/Temp timing to 20 seconds (FIRST PRODUCTION VERSION)
* Michael Pearce	09/10/2010	1.03	Changed F1 and F2 to 't' and 'b'
* Michael Pearce	09/10/2010	1.02	Some clean up. Add version display on boot
* Michael Pearce	09/09/2010	1.01	Fix Timing issues, add C/F selection
* Michael Pearce	09/08/2010	1.00	Turn Touch Demo into the AAAA Clock demo
* Keith Curtis      08/19/2010  0.00	Simple touch code for demo
******************************************************************************/
//*****************************************************************************
// Include and Header files
//*****************************************************************************
#define VERSION 106			// Format: MMmm  MM=Major mm=minor decimal inserted between

#include "pic.h"
#include "GenericTypeDefs.h"
#include "main.h"
#include "lcd.h"


/*****************************************************************************
*                            PIC Configuration
*****************************************************************************/
__CONFIG(FOSC_INTOSC&WDTE_OFF&PWRTE_ON&MCLRE_ON&CP_OFF&CPD_OFF&BOREN_OFF&CLKOUTEN_OFF&IESO_OFF&FCMEN_ON);
__CONFIG(WRT_OFF&PLLEN_OFF&STVREN_OFF&BORV_19&LVP_OFF);


/*****************************************************************************
*                           Definitions and Equates
*****************************************************************************/
// System flag defines
#define block   _sys_flags._block               // alias for block of sys flags
#define tick    _sys_flags._flags._tick         // alias for system time tick

// Cap touch defines
#define update  _sys_flags._flags._update       // alias for system update flag
#define first   _sys_flags._flags._first        // alias for avg preload flag
#define BTN1    _sys_flags._flags._BTN1         // alias for system Button 1 state
#define BTN2    _sys_flags._flags._BTN2         // alias for system Button 2 state

/*****************************************************************************
*                          Global Variables
*****************************************************************************/

BYTE    CS_statevar;        // state variable for cap touch system
WORD    raw[2];             // raw value variables for cap touch
WORD    avg[2];             // current state of environment
signed int	thold[2];			// thresholds
WORD	avgrst[2];			// Average Reset counter (If key held for too long)
WORD    temp_avg;           // temporary variable for cap touch system

// system flags
union {
    BYTE    _block;
    struct {
        BYTE    _tick:1;
        BYTE    _first:1;
        BYTE    _update:1;
        BYTE    _BTN1:1;
        BYTE    _BTN2:1;
        BYTE    unused:3;
    } _flags;
} _sys_flags;

char TickCount=0;			// Counts ticks, 8 per second then clears
char BatTempSel=0;			// Indicates if sampling Battery or Temperature
char AMPM;  				// 0 = 12Hr Clock with A and P. 1 = 24Hr Clock with 10 sec
char DEGCF;					// 0 = Degrees C,  1= Degrees F
char SetupState=0;			// State for running (0) or setup modes (1) 

unsigned int BatteryV, TemperatureV;// Battery and Temperature voltage results
unsigned int Time24,Time;			// Time 24/12hr results
unsigned char Sec,Min,Hrs;			// Time seperated

#ifdef USE_ALARM
unsigned char AlarmMin,AlarmHrs;	// Alarm Time seperated
unsigned int Alarm24;				// Alarm Time 24Hr Format
bit AlarmEnabled;					// Alarm Enable/Disable bit
#endif

#define BAT_TEMP_COUNTER_PERIOD 80; // Test every 80/4=10 seconds to save power and 
									// give more time to the cap sense
unsigned char BAT_TEMP_COUNTER=0;	// Counter for Bat/Temp sampling - only do once in a while

unsigned char CalibrationMode=0;	// Calibration Mode State

/*****************************************************************************
*                       Local Function Prototypes
*****************************************************************************/
void Init (void);           				// configure system peripherals and variables
void cap_Sense(void);       				// perform cap touch function
void IncTime(void);							// Increments time
void ShowNumber(unsigned int num, char dp);	// Displays a number between 0 and 9999 on the LCD
void BatteryDisplay(void);					// Displays the battery voltage
void TemperatureDisplay (void);				// Displays the temperature
void TimeDisplay(void);						// Displays the time
void Setup(void);							// Runs the setup state machine options
void Beep(unsigned int length);				// Makes a BEEP if hardware is attached and alarm enabled
void CapSenseCalibrate(void);				// Runs the cap sense Calibration and detects jam condition


#ifdef USE_ALARM
void AlarmDisplay(void);					// Alarm time Display function
void AlarmCheck(void);						// Alarm Function
#endif


/*****************************************************************************
* 									MAIN 
*****************************************************************************/
void main (void)
{
	
	unsigned char Rotate,SetupDelay;


	Init();						// Initialise the Hardware
	lcd_init();					// Initialise the LCD Peripheral
	IncTime();					// Increment the time to force a time value update

	ShowNumber(VERSION,0x82);   // Display version on power up no leading 0
	
	// Display Version for at least one second
	for(Rotate=8;Rotate>0;Rotate--)
	{
		while(tick==0);			// Wait for a tick to occur - 8 per second
		tick=0;
	}

	AMPM=0;				// Start in AM/PM mode
	Rotate=0;			// Clear data rotation count
	SetupDelay=0;		// Clear delay count to enter the Setup
	

    while(1)
    {
		if(tick) // tick occurs 8x per second. 
		{
			tick=0;
						
			TickCount++;
			if(TickCount>=8)  	// 8 ticks per second
			{
				IncTime();		// Increment the Clock Time
				TickCount=0;	
				SEG_COLON=0;	// Clear the Colon
	
				Rotate++; 		// Rotates between time,temp and bat V
				if(Rotate >=20)Rotate=0;

				SEG_MCHP^=1;	// Toggles the Microchip Technology Inc Logo
			}
			if(TickCount==4) SEG_COLON=1; // Sets Colon 
			
		}
		
		if (update > 0)    // new button data is available
    	{
            update = 0;
           	if(BTN1)SEG_F4=1; // Show state of Set button on the LCD
			else SEG_F4=0; 
			
			if(BTN2)SEG_F3=1; // Show state of Mode button on LCD			
			else SEG_F3=0;
				
			CapSenseCalibrate(); // Enter Calibration and auto fix state machine
						
		} // END if(update)
			


		if(SetupState==0) // Run Normally
		{
			if(BTN2 && !BTN1)	// Test for only MODE being held for > 2 seconds
			{
				SetupDelay++;
				if(SetupDelay > 16) 
				{
					SetupState=1;
					SetupDelay=0;
				}
			}
			else	SetupDelay=0;

	        
         	// Update the display	
			if(Rotate==1||Rotate==2||Rotate==3||Rotate==4)TemperatureDisplay();				
			else TimeDisplay();
			
		}
		else  // if(SetupState) else
		{
			if(BTN1 && BTN2)		// If both buttons pressed - Exit setup mode
			{
				SetupState=0;
			}
			else
			{
				Setup();	// Run the Setup state machine if correct.
			}
		} //  END if(SetupState) else

		#ifdef USE_ALARM
		AlarmCheck();
		#endif




		SLEEP(); // Sleep after every pass to minimize current use
		NOP();
	}
}


/*****************************************************************************
*							 INTERRUPT
*
* TMR1 used as a means of generating interrupts at 8 times a second.  
******************************************************************************/
void interrupt isr(void)
{
	if(TMR1IF == 1)
	{
		TMR1H=TMR1H_LOAD;	// Only need to reload High.
		TMR1IF=0;			// Clear Flag
		tick=1;				// Indicate Timer Tick
		cap_Sense();		// Do Cap sense and ADC sampling
	}
}
        


/******************************************************************************
* Function: void Init (void)
*
* Overview: This function is used by the main program for general setup
*          
* Input:    None
*
* Output:   None
*
******************************************************************************/
void Init (void)
{
	
	OSCCON=OSCCON_LOAD;

	T1CON=T1OSCSTART;
    while(T1OSCR==0);  			// Wait for Timer 1 oscillator to start up and run

    // preset system variables
    block       = 0b00000000;	// reset system flags
    CS_statevar = 0;			// reset statevariable for cap touch
    first       = 1;			// preload average with initial values


	Sec=Min=Hrs=0;				// Preset time values
	Time24=Time=0;

#ifdef USE_ALARM
	AlarmMin=AlarmHrs=0;		// Preset Alarm values
	Alarm24=0;
	AlarmEnabled=1;

								// PWM Configuration
	CCP3CON=CCP3CON_LOAD;		// PWM mode, output steering LSBs=0
 	PSTR3CON=PSTR3CON_LOAD;		// Steer output to P1C
 	PR2=PR2_LOAD;				// 32.768KHz/16 = 2.048KHz
	T2CON=T2CON_LOAD;			// Pre & Post 1:1 and turn ON
	CCPR3L=CCPR3L_LOAD;			// Load zero to start with so no output
#endif


    //**** Configure GPIO ****
    LATA    = LATA_LOAD;		
    LATB    = LATB_LOAD;
    LATC    = LATC_LOAD;

    TRISA   = TRISA_LOAD;
    TRISB   = TRISB_LOAD;
    TRISC   = TRISC_LOAD;

    ANSELA  = ANSELA_LOAD;
    ANSELB  = ANSELB_LOAD;

    //**** Timer1 configuration ****
    TMR1L   = TMR1L_LOAD;				// Set Time for initial count
    TMR1H   = TMR1H_LOAD;
    T1CON   = T1CON_LOAD;				// T1OSC, no pscale, OSC on, sync on, TMR1 on
    T1GCON  = T1GCON_LOAD;				// disable TMR1 gate

    // ADC configuration for cap touch
    ADCON0   = ADC_BTN2;				// startup with button 2 selected AN10
	ADCON1 = ADCON1_LOAD_CAPS;

	FVRCON=FVRCON_LOAD;	// For 2.048V ref


	thold[0]=thold[1]=threshold;		// Load Default thresholds
	

    // Start cap sense with first button
    BTN1_out;							// charge ADC Chold
    BTN1_high;
    ADCON0   = ADC_BTN1;
    GO_nDONE = 1;						// disconnect ADC Chold
    BTN1_low;							// discharge sensor BTN 1
    BTN1_in;							// disconnect output driver
    GO_nDONE = 0;						// reconnect ADC Chold to sensor
    GO_nDONE = 1;						// start conversion of value

	TMR1IF=0;
	TMR1IE=1;

    PEIE    = 1;						// enable peripheral interrupts
    GIE     = 1;						// enable system interrupts
}

/******************************************************************************
* Function: void cap_Sense (void)
*
* Overview: This function performs the capacitive touch function on 2 buttons
*           It also does the measurement of the battery and the temperature. 
*
* Input:    None
*
* Output:   None
*
******************************************************************************/
void cap_Sense(void)
{
	switch(CS_statevar)
	{
    	case 0:
    
        raw[0] = (ADRESH << 8) + ADRESL;			// store previous value
        BTN2_out;									// charge ADC Chold
        BTN2_high;
        ADCON0   = ADC_BTN2;
        GO_nDONE = 1;								// disconnect ADC Chold
        BTN2_low;									// discharge sensor BTN 2
        BTN2_in;                         			// disconnect output driver
        GO_nDONE = 0;								// reconnect ADC Chold to sensor
        GO_nDONE = 1;								// start conversion of value
        CS_statevar = 1;
		if(BatTempSel==1 && BAT_TEMP_COUNTER==0)TEMP_EN=1;
		else TEMP_EN=0;
		break;
	
		case 1:
    		// Store the buttons data
    	    raw[1] = (ADRESH << 8) + ADRESL;    	// store previous value

			if(BAT_TEMP_COUNTER > 0) BAT_TEMP_COUNTER--;
			if(BAT_TEMP_COUNTER == 0) 				// Skip case if not time to read sensors
			{
				//**** Start the conversion on the Battery or Temperature ****
				ADON=0;								// Turn ADC OFF
				ADCON1 = ADCON1_LOAD_2048;			// Set reference to 2.048V internal

				FVREN=1;							// Turn Reference On
				while(FVRRDY==0);					// Wait for reference to get stable

				if(BatTempSel==0)					// Select Battery or Temeprature
				{
		 			ADCON0 = ADC_SEL_BATTERY;		// Select the battery channel turn ADON
				}
				else
				{
					ADCON0   = ADC_SEL_TEMPERATURE;	// Select the temperature channel turn ADON
				}
		
     			NOP();NOP();						// Min delay before starting conversion
        		NOP();NOP();
				CS_statevar =2;
        		GO_nDONE = 1;       				// start conversion of Battery or Temperature
				break;
			}
						

    	case 2:
			if(BAT_TEMP_COUNTER == 0) 				// skip this if not time to read sensors
			{
    			FVREN=0; 							// Power the VREF off

				if(BatTempSel ==0)
				{
					BatteryV=ADRES<<1;				// Store and connvert ADCRES as battery voltage
					BatTempSel=1;
				}
				else
				{
					TEMP_EN=0;						// Power off temperature sensor
					TemperatureV=ADRES<<1;			// Store and convert ADRES as Temperature voltage
					BatTempSel=0;					// Reset Battery/temperature selector
					BAT_TEMP_COUNTER=BAT_TEMP_COUNTER_PERIOD; // Restart the wait period
				}

				ADON=0;								// Turn ADC OFF
				ADCON1 = ADCON1_LOAD_CAPS;			// Set up ADC for Cap sense
				
			}


			// Start conversion on the Button
	   		BTN1_out;								// charge ADC Chold
    	    BTN1_high;
  			ADCON0   = ADC_BTN1;
   	     	GO_nDONE = 1;							// disconnect ADC Chold
   	     	BTN1_low;								// discharge sensor BTN 1
        	BTN1_in;								// disconnect output driver
        	GO_nDONE = 0;							// reconnect ADC Chold to sensor
        	GO_nDONE = 1;							// start conversion of value
        	CS_statevar = 0;

        	update   = 1;							// both buttons scanned this pass
			break;


		default:
			CS_statevar = 0;

	}


	//**** Run Cap sense Averaging *****
    if (update > 0)									// if both are done do press detect
    {
        if (first > 0)								// on first pass through the loop
        {											// preset the average variables
            avg[0] = raw[0] << 5;
            avg[1] = raw[1] << 5;
	 		BTN1 = 0;
            BTN2 = 0;
            first  = 0;
        }
        else 										// if not first, check for press
        {
            BTN1 = 0;
            BTN2 = 0;

            temp_avg = avg[0] >> 5;
            if ((temp_avg - thold[0]) > raw[0]) BTN1 = 1;		// if pressed set flags
            if (BTN1 == 0) avg[0] = avg[0] - temp_avg + raw[0];	// if not average the raw value

            temp_avg = avg[1] >> 5;
            if ((temp_avg - thold[1]) > raw[1]) BTN2 = 1;
            if (BTN2 == 0) avg[1] = avg[1] - temp_avg + raw[1];
        }
    }

}



/******************************************************************************
* Function: void IncTime (void)
*
* Overview: This function increments the time by 1 second and
*			updates minutes and hours as required.
*			Time is stored in 24 hour format and AM/PM handled elseware
*
* Input:    None
*
* Output:   None
*
******************************************************************************/
void IncTime(void)
{
	Sec++;
	if(Sec >59)				// Check for Seconds -> Minutes overflow
	{
		Sec=0;
		Min++;
		if(Min >59)			// Check for Minutes -> Hours Overflow
		{
			Min=0;
			Hrs++;
			if(Hrs > 23)	// Check for Hours -> Day Overflow
			{
				Hrs = 0;
			}

		}
	}
	//**** Create the 16 bit Hrs/Minutes ****
	Time24=Hrs*100;
	Time24+=Min;
	Time=Time24;
	if(AMPM==0)						// Check for 12Hr format, and convert down
	{
		if(Time > 1299) Time-=1200;	// Convert to 12 Hour for PM
		if(Time <	99) Time+=1200; // Create Midnight for AM
	}
}


/******************************************************************************
* Function: void ShowNumber (void)
*
* Overview: This displays a number on the screen
*			Positive values only 0 to 9999
*			Selectable decimal point
*			Leading or no Leading zeros
*
* Input:    unsigned int num  - Value between 0 and 9999
*
*			char db - location of decimal point 0=none, 1=left, 2=center, 4=right
*					- Can add together to get a combination of locations
*					- add 0x80 to remove leading zeros
* Output:   None
*
******************************************************************************/
void ShowNumber(unsigned int num, char dp)
{
	unsigned int temp;
	unsigned char lead=0;

	if(dp & 0x80)lead=1;	// If set remove leading zeros

	if(num > 9999) 			// If greater than 9999 then we have an Error
	{
		// Display Err
		lcd_putc(CHAR_E,3,0);
		lcd_putc(CHAR_r,2,0);
		lcd_putc(CHAR_r,1,0);
		lcd_putc(CHAR_SPACE,0,0);
		return;
	}

	temp=num/1000;							// Thousands digit
	if(lead==1 && temp==0 && (dp&1)==0)
	{
		lcd_putc(CHAR_SPACE, 3,0);
	}
	else
	{
		lcd_putc(temp, 3,dp&1);
		lead=0;
	}
	num-=(temp*1000);

	temp=num/100;							// Hundreds Digit
	dp=dp>>1;
	if(lead==1 && temp==0 && (dp&1)==0)
	{
		lcd_putc(CHAR_SPACE, 2,0);
	}
	else
	{
		lead=0;
		lcd_putc(temp, 2,dp&1);
	}
	num-=(temp*100);

	temp=num/10;							// Tens Digit
	dp=dp>>1;
	if(lead==1 && temp==0 && (dp&1)==0)
	{
		lcd_putc(CHAR_SPACE, 1,0);
	}
	else
	{
		lead=0;
		lcd_putc(temp,1,dp&1);
	}
	num-=(temp*10);

	// Dont check for lead - always print he last zero.
	lcd_putc(num,0,0);						// Ones Digit
}	

/******************************************************************************
* Function: void BatteryDisplay (void)
*
* Overview: This function updates the battery status symbols
*			And displays the Battery voltage on the screen
*
* Input:    None
*
* Output:   None
*
******************************************************************************/
void BatteryDisplay(void)
{
	// Update Segments
	SEG_BAT2=SEG_BAT3=SEG_BAT4=0;
	SEG_BAT1=1;								// Battery Outline always displayed
	SEG_COLON=0;
	
	// No Bars indicates battery is below BAT_LEVEL_MIN and should be replaced
	if(BatteryV > BAT_LEVEL_MIN)SEG_BAT2=1;	// One Bar
	if(BatteryV > BAT_LEVEL_MED)SEG_BAT3=1;	// Two Bars
	if(BatteryV > BAT_LEVEL_MAX)SEG_BAT4=1;	// Three Bars


	// Display Voltage on the display
	ShowNumber(BatteryV,1);					// Print the voltage
		
	lcd_putc(CHAR_b,4,0); 					// Display a "b" in the top right corner
	

	if(BatteryV < BAT_LEVEL_MIN)Beep(100);	// Alert user to Low Battery

}

/******************************************************************************
* Function: void TemperatureDisplay (void)
*
* Overview: This displays the temperature on the screen
*
* Input:    None
*
* Output:   None
*
******************************************************************************/
void TemperatureDisplay (void)
{	
	unsigned int result, minus_flag;
	SEG_COLON=0;
	

	result = TemperatureV;	
	minus_flag = 0;
	if (result < T_OFFSET_ZERO){
		result = T_OFFSET_ZERO - result;
		minus_flag = 1;
	} else 
		result -= T_OFFSET_ZERO;        // Shift by the 0 degree offset (400mV for MCP9701)
	result = result * 10;			// scale up so we can do integer math to calc temperature
    result = result / T_DIVISOR;	// 19.5mV/degree (195)for MCP9701 which scales down at the same time

	result*=10; 					// Shift for the display so we can put a c or f in the right digit	

	if(DEGCF==1)
	{
		// Calculate Farenheight
		result=((18*result)/10)+320;// Tf = (9/5)*Tc+32; 9/5=1.8 or 18 in our case since we are x10
		ShowNumber(result,0x80);	// Display the temperature in F
		lcd_putc(CHAR_F,0,0);	
	}
	else
	{
		// Display in Degrees C
		ShowNumber(result,0x80);	// Display temperature remove leading zeros
		lcd_putc(CHAR_c,0,0);		// Add the 'c'
	}
	
	lcd_putc(CHAR_t,4,0); 			// Display a "t" in the top right corner to indicate Temperature
	if(minus_flag) lcd_putc(CHAR_MINUS,3,0); 
}


/******************************************************************************
* Function: void TimeDisplay (void)
*
* Overview: This function displays the time in the appropiate format
*
* Input:    None
*
* Output:   None
*
******************************************************************************/
void TimeDisplay(void)
{
	unsigned char Temp;
	
	if(AMPM==0)
	{
		// 12 Hour format 
		ShowNumber(Time, 0x80);				// Display 12Hr Time with no leading zeros
		if(Hrs > 11)lcd_putc(CHAR_P,4,0);	// Display an A or P in the top right corner
		else lcd_putc(CHAR_A,4,0);
	}
	else
	{
		// 24 Hour format 
		ShowNumber(Time24, 0);				// Display 24 Hour time
	}
}


/******************************************************************************
* Function: void Setup (void)
*
* Overview: This function lets user set up time and other things
*
* Input:    None
*
* Output:   None
*
******************************************************************************/
#define KEY_CHECK 4  // Times through before checking a key and blinking display
void Setup(void)
{
	static char Blink=0;
	
	if(SetupState < 7)
	{
		TimeDisplay();  		// Display for Time based settings
	}
	else if (SetupState < 10)
	{	
		TemperatureDisplay();	//  Display for Temperature settings
		SEG_COLON=0;			// Remove the auto Colon 
	}
	#ifdef USE_ALARM
	else
	{
		AlarmDisplay(); 		// Display for Alarm based settings
	}
	#endif
	
	

	Blink++;
	if(Blink < KEY_CHECK) return;
	Blink=0;
	
	switch(SetupState)
	{
		case 1: // First time in - Blink  Hrs, wait for Mode button Release
			lcd_putc(CHAR_SPACE,3,0);
			lcd_putc(CHAR_SPACE,2,0);
			if(BTN2==0) SetupState++;
			break;

		case 2: // Set Hours
			lcd_putc(CHAR_SPACE,3,0);
			lcd_putc(CHAR_SPACE,2,0);
			
			if (BTN1==1)
			{
				Hrs++;
				if(Hrs > 23)Hrs=0;
				Sec=0; // Clear seconds only when changing the time
			}
			else if(BTN2==1)
			{
				SetupState++;
			}
			
			break;

		case 3: // Blink Minutes - wait for Mode to be released
			lcd_putc(CHAR_SPACE,1,0);
			lcd_putc(CHAR_SPACE,0,0);
			if(BTN2==0) SetupState++;
			
			break;

		case 4: // Set Minutes
			lcd_putc(CHAR_SPACE,1,0);
			lcd_putc(CHAR_SPACE,0,0);
			
			if (BTN1==1)
			{
				Min++;
				if(Min > 59)Min=0;
				Sec=0;				// Clear seconds only when changing the time
			}
			else if(BTN2==1)
			{
				SetupState++;
			}
			break;


		case 5: // Blink AM/PM/24 - wait for Mode to be released
			lcd_putc(CHAR_SPACE,4,0);
			if(BTN2==0) SetupState++;
			break;


		case 6:	// Change AM/PM/24
			lcd_putc(CHAR_SPACE,4,0);
			if (BTN1==1)
			{
				if(AMPM==0)
				{
					if(Hrs < 12)Hrs+=12;	// Move to PM
					else AMPM=1;			// Change to 24Hr Format
				}
				else
				{
					if(Hrs > 11)Hrs-=12; 	// Move to AM
					AMPM=0;
				}


			}
			else if(BTN2==1)
			{
				SetupState++; 	
			}
			break;

		case 7: // C or F Wait for release of button
			lcd_putc(CHAR_SPACE,0,0); 		// blank C/F character;
			if(BTN2==0) SetupState++;
			break;

		case 8: // C or F 
			lcd_putc(CHAR_SPACE,0,0);		 // blank C/F character
			if (BTN1==1)
			{
				if(DEGCF==0) DEGCF=1;
				else DEGCF=0;

			}
			else if(BTN2==1)
			{
				SetupState++; 				// Move to Next state
			}
			break;

		case 9: // Wait for release of button to Exit
			if(BTN2==1) break;
			SetupState++;
			
// Following options are for the ALARM setting #define USE_ALARM in main.h
#ifdef USE_ALARM
		case 10: // Blink Alarm Hrs
			lcd_putc(CHAR_SPACE,3,0);
			lcd_putc(CHAR_SPACE,2,0);
			if(BTN2==0) SetupState++;
			break;

		case 11: // Set Alarm Hours
			lcd_putc(CHAR_SPACE,3,0);
			lcd_putc(CHAR_SPACE,2,0);
			
			if (BTN1==1)
			{
				AlarmHrs++;
				if(AlarmHrs > 23)AlarmHrs=0;
			}
			else if(BTN2==1)
			{
				SetupState++;
			}
			break;

		case 12: // Blink Alarm Minutes
			lcd_putc(CHAR_SPACE,1,0);
			lcd_putc(CHAR_SPACE,0,0);
			if(BTN2==0) SetupState++;
			break;

		case 13: // Set Alarm Minutes
			lcd_putc(CHAR_SPACE,1,0);
			lcd_putc(CHAR_SPACE,0,0);
			
			if (BTN1==1)
			{
				AlarmMin++;
				if(AlarmMin > 59)AlarmMin=0;
			}
			else if(BTN2==1)
			{
				SetupState++;
			}
			break;

		case 14: // Blink On/Off
			lcd_putc(CHAR_SPACE,4,0);
			if(BTN2==0) SetupState++;
			break;


		case 15: // Turn On or Off
			lcd_putc(CHAR_SPACE,4,0);
		
			if (BTN1==1)
			{
				if(AlarmEnabled)AlarmEnabled=0;
				else AlarmEnabled=1;
			}
			else if(BTN2==1)
			{
				SetupState++;
			}
			if(AlarmEnabled)Beep(100);
			break;

		

#endif


		default: 
			if(BTN2==1)break; // Wait for Button to be released for the Exit
			SetupState=0; 	  // Exit
		


	} // END: switch(SetupState)

	IncTime();   	// Updates the time if anything was changed. This updates the values
					// Since SEC is set to 0 to stop the clock this will not cause a roll over
}



/******************************************************************************
* Function: void AlarmCheck (void)
*
* Overview: This function test the alarm time and sounds the alarm if appropiate
*			It will do this 32 times (aprox 4 seconds)
*
* Input:    None
*
* Output:   None
*
******************************************************************************/
#ifdef USE_ALARM
void AlarmCheck(void)
{
	static char alarmcount;
	if(Alarm24==Time24 && AlarmEnabled==1)	// Use 24hr formats to check Alarm 
	{
		if(Sec==0)alarmcount=0;				// clear the alarm count if in the first second
		if(alarmcount<32)					
		{
 			Beep(1000);						// Make some noise
			alarmcount++;					// Increment the alarm counter till we are done
		}
	}

}
#endif

/******************************************************************************
* Function: void AlarmDisplay (void)
*
* Overview: This function displays the Alarms time in 24 Hour format only
*
* Input:    None
*
* Output:   None
*
******************************************************************************/
#ifdef USE_ALARM
void AlarmDisplay(void)
{

	SEG_F1=1;SEG_F2=0;

	//Calculate 24 Hour Alarm Time (This updates the actual alarm time as well)
	Alarm24=(AlarmHrs*100) + AlarmMin;

	// 24 Hour format - display seconds with leading zeros
	ShowNumber(Alarm24, 0);

	// Show n for On or a o for Off
	if(AlarmEnabled) lcd_putc(CHAR_n,4,0);
	else lcd_putc(CHAR_o,4,0);
	
}
#endif // USE_ALARM


/******************************************************************************
* Function: void Beep (void)
*
* Overview: This function Does a short Beep.
*			Only works is a Piezo is attached, and the Alarm is enabled
*
* Input:    None
*
* Output:   None
*
******************************************************************************/
void Beep(unsigned int length)
{
#ifdef USE_ALARM
	PR2=32;
	CCPR3L=16;
	while(length--);
	CCPR3L=0;
#endif	// USE_ALARM
}

/******************************************************************************
* Function: void CapSenseCalibrate (void)
*
* Overview: Runs the cap sense Calibration and detects jam condition
*
*
* Input:    None
*
* Output:   None
*
******************************************************************************/
void CapSenseCalibrate(void)
{
	static unsigned char CalCounter;
	static unsigned int Press[2];


	// Test for an individually jammed buttons
	if(BTN1)
	{
		avgrst[0]++;
		if(avgrst[0]>=AVGRST_MAX)
		{
			first=1;				// Reset the cap sense averages
			SetupState=0;			// Force Exit of any setup routine
		}
	}
	else
	{
		avgrst[0]=0;
	}

	if(BTN2)
	{
		avgrst[1]++;
		if(avgrst[1]>=AVGRST_MAX)
		{
			first=1;				// Reset the cap sense averages
			SetupState=0;			// Force Exit of any setup routine
		}
	}
	else
	{
		avgrst[1]=0;
	}



	// Check for Calibration mode or a two button Jam	
	switch(CalibrationMode)
	{
		case 0:
			if(BTN1 && BTN2) CalibrationMode++;
			return;

		case 1:		// First Entry - store the raw data for the press level
			if(BTN1 && BTN2)
			{
				Press[0]=raw[0];
				Press[1]=raw[1];
				CalCounter=0;
				CalibrationMode++;
			}
			else	// Only a Glitch so ignore
			{
				CalibrationMode=0;
			}
			break;
		

		case 2:		// Start timing the dual hold to capture a lock on
			SEG_F1=1;				// Indicate calibration Mode
			
				
			if(BTN1 && BTN2)		// Inc counter if both buttones pressed
			{
				CalCounter++;
				if(CalCounter > 20)			// More than 5 seconds of both held - must be error
				{
					first=1;				// Reset the cap sense averages
					SetupState=0;			// Force Exit of any setup routine	
					CalibrationMode++;		// Exit the calibration
					break;
				}
			}
			else
			{
				// Wait for both buttons released then do the calibration
				if(BTN1==0 && BTN2==0)
				{
					thold[0]=(raw[0]-Press[0])/2;	
					if(thold[0]< thresholdmin)thold[0]=thresholdmin;
					if(thold[0]> thresholdmax)thold[0]=thresholdmax;

					thold[1]=(raw[1]-Press[1])/2;
					if(thold[1]< thresholdmin)thold[1]=thresholdmin;
					if(thold[1]> thresholdmax)thold[1]=thresholdmax;



					CalibrationMode++;
					first=1;				// Reset the cap sense averages
				}
									
			}
			break;
			




		default:	// Exit
			CalibrationMode=0;
			SEG_F1=0;
			break;
			
		
	}			

}





/****** END OF main.c *******/

