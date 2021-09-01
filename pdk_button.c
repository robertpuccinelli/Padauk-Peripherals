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


LOGIC:

Neg edge detector

byte previous, current, active, debounce;

current  = read_pins &~ debounce;
active   = previous &~ current;
previous = current;
debounce = active | debounce;


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


STATIC BYTE temp_byte;

STATIC BYTE button_debounce_a;
STATIC BYTE button_debounce_b;
STATIC BYTE button_debounce_c;

STATIC BYTE &button_current_a = temp_byte;
STATIC BYTE &button_current_b = temp_byte;
STATIC BYTE &button_current_c = temp_byte;

STATIC BYTE button_previous_a;
STATIC BYTE button_previous_b;
STATIC BYTE button_previous_c;

BYTE button_active_a;
BYTE button_active_b;
BYTE button_active_c;

BYTE button_enabled_a;
BYTE button_enabled_b;
BYTE button_enabled_c;

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
		button_enabled_a = BTN_PA;      // Grab enabled pin list
		PAC = PAC & ~button_enabled_a;  // Set control register to input
		PAPH = PAPH | button_enabled_a; // Set pull high register
		PADIER = BTN_PA & BTN_PA_W;  // Set pin interrupt state if used by BTN
		button_debounce_a = 0;          // Reset debounce list
		button_previous_a = 0;          // Reset previous state
		button_active_a   = 0;          // Reset active button list
	#endif

	// PORT B
	#if BTN_USE_PB
		button_enabled_b = BTN_PB;
		PBC = PBC & ~button_enabled_b;
		PBPH = PBPH | button_enabled_b;
		PBDIER = BTN_PB & BTN_PB_W;
		button_debounce_b = 0;					
		button_previous_b = 0;
		button_active_b   = 0;
	#endif

	// PORT C
	#if BTN_USE_PC
		button_enabled_c = BTN_PC;
		PCC = PCC & ~button_enabled_c;
		PCPH = PCPH | button_enabled_c;
		PCDIER = BTN_PC & BTN_PC_W;
		button_enabled_c = BTN_PC;
		button_debounce_c = 0;				
		button_previous_c = 0;
		button_active_c   = 0;
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
			// Debounce forces pins to be low until debounce cleared
			// Next, buttons not enabled are set to HIGH
			// button_active represents detection of falling edge. Cleared on next poll
			button_current_a = (PA & ~button_debounce_a) | ~(button_enabled_a & BTN_PA);
			button_active_a = button_previous_a &~ button_current_a;
			button_previous_a = button_current_a;
			button_debounce_a = button_active_a | button_debounce_a;
			if (button_active_a) trigger_debounce = 1;
		#endif

		// PORT B
		#if BTN_USE_PB
			button_current_b = (PB & ~button_debounce_b) | ~(button_enabled_b & BTN_PB);
			button_active_b = button_previous_b &~ button_current_b;
			button_previous_b = button_current_b;
			button_debounce_b = button_active_b | button_debounce_b;
			if (button_active_b) trigger_debounce = 1;
		#endif

		//  PORT C
		#if BTN_USE_PC
			button_current_c = (PC & ~button_debounce_c) | ~(button_enabled_c & BTN_PC);
			button_active_c = button_previous_c &~ button_current_c;
			button_previous_c = button_current_c;
			button_debounce_c = button_active_c | button_debounce_c;
			if (button_active_c) trigger_debounce = 1;
		#endif

		if (trigger_debounce) Start_Debounce_Timer();		// Restart the debounce timer if flagged
	}
}



// INTERRUPT
void Button_Debounce_Interrupt(void)
{
	if (button_module_initialized){

		// If pins being watched are still low, mark them active
		$ BTN_TIMER_CTL STOP;

		// PORT A
		#if BTN_USE_PA
			button_debounce_a = 0;
		#endif

		// PORT B
		#if BTN_USE_PB
			button_debounce_b = 0;
		#endif

		// PORT C
		#if BTN_USE_PC
			button_debounce_c = 0;
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
			button_enabled_a = BTN_PA;        // Grab original list of button pins
			PAPH = PAPH & ~button_enabled_a;  // Set buttons to NOPULL		
			$ PADIER 0;                   // Prevent pins from waking system, if set
			button_active_a = 0;              // Clear active flags
		#endif

		// PORT B
		#if BTN_USE_PB
			button_enabled_b = BTN_PB;
			PBPH = PBPH & ~button_enabled_b;
			$ PBDIER 0;
			button_active_b = 0;					
		#endif

		// PORT C
		#if BTN_USE_PC
			button_enabled_c = BTN_PC;
			PCPH = PCPH & ~button_enabled_c;
			$ PCDIER 0;
			button_active_c = 0;					
		#endif
	
		button_module_initialized = 0;	// Disable functions in module
	}
}