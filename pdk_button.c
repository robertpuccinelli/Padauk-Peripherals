/* pdk_button.c
   
Button peripheral utilities.

Buttons are configured to be digital input, pull high and report debounce filtered
presses through the active_x byte. After ports/pins have been set in system_settings.h,
the buttons can be individually enabled/disabled with the button_enabled_x byte.

The compiler does not support dereferencing function pointers, which makes implementing 
callbacks pretty challenging. Currently, callbacks need to be implemented in the user 
program, but suggestions for a better implementation are welcome.

Place the Button_Debounce_Interrupt function under the appropriate timer interrupt flag
in the user program.


NOTE:

	The bits in the port interrupt enable register are not individually addressable
	so BTN_WAKE_SYS in system_settings.h will overwrite any previous PxDIER setting.
	If Button_Initialize is called last, PxDIER == BTN_Px when BTN_WAKE_SYS = 1. To 
	enable button wakeups and non button wakeups, in the user program:
	
		if (BTN_WAKE_SYS) $ PxDIER (BTN_Px | YourBitMask)

	Similarly, releasing the buttons sets the PxDIER register to 0.


ROM Consumed : 116B / 0x74  -  1 Port
RAM Consumed :  18B / 0x12  -  1 Port

Each Additional Port
ROM Consumed + 46B / 0x2E
RAM Consumed +  4B / 0x04


This software is licensed under GPLv3 <http://www.gnu.org/licenses/>.
Any modifications or distributions have to be licensed under GPLv3.
No warranty of any kind and copyright holders cannot be held liable.
Licensees cannot remove copyright notices.

Copyright (c) 2021 Robert R. Puccinelli
*/

#include "system_settings.h"


//======================//
// VARIABLES AND MACROS //
//======================//

BYTE button_flags = 0;
BIT  button_module_initialized : button_flags.?;
BIT	 trigger_debounce : button_flags.?;


BYTE falling_edge_a;
BYTE falling_edge_b;
BYTE falling_edge_c;

BYTE watchlist_a;
BYTE watchlist_b;
BYTE watchlist_c;

BYTE active_a;
BYTE active_b;
BYTE active_c;

BYTE button_enabled_a;
BYTE button_enabled_b;
BYTE button_enabled_c;

STATIC BYTE temp_byte;

// Number of counts to wait before firing debouncer interrupt
TIMER_BOUND		=>	BTN_TIMER_FREQ / (BTN_TIMER_DIV + 1) / (1000 / BTN_DEBOUNCE_T);


//==================//
// STATIC FUNCTIONS //
//==================//

void Start_Debounce_Timer(void)
{
	BTN_TIMER_CNT = 0;
	$ BTN_TIMER_CTL BTN_TIMER_CLK;
}


//===================//
// PROGRAM FUNCTIONS //
//===================//


// INITIALIZE
void Button_Initialize(void)
{
	// Set pins to button state and allow button function calls

	// Configure debounce timer
	$ BTN_TIMER_CTL STOP;
	BTN_TIMER_CNT = 0;
	BTN_TIMER_BND = TIMER_BOUND;
	BTN_TIMER_SCL = BTN_TIMER_DIV;

	// PORT A
	#if BTN_USE_PA
		button_enabled_a = BTN_PA;						// Grab enabled pin list
		PAC = PAC & ~button_enabled_a;					// Set control register to input
		PAPH = PAPH | button_enabled_a;					// Set pull high register
		if (BTN_WAKE_SYS) PADIER = button_enabled_a;	// Set pin interrupt state if used by BTN
		falling_edge_a = 0;								// Reset falling edge detection
		watchlist_a = 0;								// Reset falling edge watchlist
		active_a = 0;									// Reset active button list
	#endif

	// PORT B
	#if BTN_USE_PB
		button_enabled_b = BTN_PB;
		PBC = PBC & ~button_enabled_b;
		PBPH = PBPH | button_enabled_b;
		if (BTN_WAKE_SYS) PBDIER = button_enabled_b;
		falling_edge_b = 0;					
		watchlist_b = 0;
		active_b = 0;
	#endif

	// PORT C
	#if BTN_USE_PC
		button_enabled_c = BTN_PC;
		PCC = PCC & ~button_enabled_c;
		PCPH = PCPH | button_enabled_c;
		if (BTN_WAKE_SYS) PCDIER = button_enabled_c;
		button_enabled_c = BTN_PC;
		falling_edge_c = 0;				
		watchlist_c = 0;
		active_c = 0;
	#endif

	INTRQ.BTN_TIMER = 0;					// Ensure that timer interrupt is cleared
	INTEN.BTN_TIMER = 1;					// Enable timer interrupt
	button_module_initialized = 1;			// Enable other functions in module
}


// POLL
void Button_Poll(void)
{
	if (button_module_initialized){

		// Find falling edges and start debouncing timer
		trigger_debounce = 0;

		// PORT A
		#if BTN_USE_PA
			falling_edge_a = (falling_edge_a ^ ~PA) & ~PA & button_enabled_a & BTN_PA;	// Find falling edge on enabled pins
			if (falling_edge_a)
			{
				watchlist_a = watchlist_a | falling_edge_a;						// Store new edges on watchlist
				trigger_debounce = 1;											// Flag the timer to start
			}
		#endif

		// PORT B
		#if BTN_USE_PB
			falling_edge_b = (falling_edge_b ^ ~PB) & ~PB & button_enabled_b & BTN_PB;
			if (falling_edge_b)
			{
				watchlist_b = watchlist_b | falling_edge_b;
				trigger_debounce = 1;
			}
		#endif

		//  PORT C
		#if BTN_USE_PC
			falling_edge_c = (falling_edge_c ^ ~PC) & ~PC & button_enabled_c & BTN_PC;
			if (falling_edge_c)
			{ 
				watchlist_c = watchlist_c | falling_edge_c;
				trigger_debounce = 1;
			}
		#endif

		if (trigger_debounce) Start_Debounce_Timer();		// Restart the debounce timer if flagged
	}
}



// INTERRUPT
void Button_Debounce_Interrupt(void)
{
	if (button_module_initialized){

		// If pins being watched are still low, mark them active
		$ BTN_TIMER_CTL STOP;								// Stop timer

		// PORT A
		#if BTN_USE_PA
			temp_byte = watchlist_a & ~PA; 					// Find real button presses 
			if (temp_byte) active_a = active_a | temp_byte;	// Add buttons to list of active callbacks
		#endif

		// PORT B
		#if BTN_USE_PB
			temp_byte = watchlist_b & ~PB;
			if (temp_byte) active_b = active_b | temp_byte;
		#endif

		// PORT C
		#if BTN_USE_PC
			temp_byte = watchlist_c & ~PC
			if (temp_byte) active_c = active_c | temp_byte;
		#endif

		INTRQ.BTN_TIMER = 0;								// Clear timer interrupt flag
	}
}


// RELEASE
void Button_Release(void)
{
	if (button_module_initialized){

		// Free pins and block button function calls

		// PORT A
		#if BTN_USE_PA
			button_enabled_a = BTN_PA;			// Grab original list of button pins
			PAPH = PAPH & ~button_enabled_a;	// Set buttons to NOPULL		
			if (BTN_WAKE_SYS) $ PBDIER 0;		// Prevent pins from waking system, if set
			active_a = 0;						// Clear active flags
		#endif

		// PORT B
		#if BTN_USE_PB
			button_enabled_b = BTN_PB;
			PBPH = PBPH & ~button_enabled_b;
			if (BTN_WAKE_SYS) $ PBDIER 0;
			active_b = 0;					
		#endif

		// PORT C
		#if BTN_USE_PC
			button_enabled_c = BTN_PC;
			PCPH = PCPH & ~button_enabled_c;
			if (BTN_WAKE_SYS) $ PCDIER 0;
			active_c = 0;					
		#endif
	
		button_module_initialized = 0;	// Disable functions in module
	}
}