/* pdk_button.c
   
General button utilities.
Buttons are configured to be digital input, pull high.

ROM Consumed : 101B / 0x65  -  1 Port
RAM Consumed :   5B / 0x05  -  1 Port

Each Additional Port
ROM Consumed + 66B / 0x42
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

BIT  button_module_initialized;
BIT	 trigger_debounce;

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

/* Need to test callback funcitonality on emulator
BYTE button_set_cb_array;
BYTE button_set_cb_num;
BYTE button_set_cb_func;

WORD button_callbacks_a; 
WORD button_callbacks_b;
WORD button_callbacks_c;

#IFDEF BTN_NUM_CB_A
	WORD button_cb_a[BTN_NUM_CB_A];
	button_callbacks_a = button_cb_a;
#ENDIF

#IFDEF BTN_NUM_CB_B
	void button_cb_b[BTN_NUM_CB_B];
	button_callbacks_b = button_cb_b;
#ENDIF

#IFDEF BTN_NUM_CB_C
	WORD button_cb_c[BTN_NUM_CB_C];
	button_callbacks_c = button_cb_c;
#ENDIF
*/

// Number of counts to wait before firing debouncer interrupt
TIMER_BOUND		=>	BTN_TIMER_FREQ / (BTN_TIMER_DIV + 1) / (1000 / BTN_DEBOUNCE_T);


//==================//
// STATIC FUNCTIONS //
//==================//

void Start_Debounce_Timer(void)
{
	BTN_TIMER_CNT = 0;
	BTN_TIMER_CTL.7 = 1;
}

/* Need to test callback functionality on emulator
void Select_Callback(void)
{
	
}
*/

//===================//
// PROGRAM FUNCTIONS //
//===================//


// INITIALIZE
void Button_Initialize(void)
{
	// Set pins to button state and allow button function calls

	// Configure debounce timer
	BTN_TIMER_CNT = 0;
	BTN_TIMER_BND = TIMER_BOUND;
	BTN_TIMER_SCL = BTN_TIMER_DIV;
	$ BTN_TIMER_CTL BTN_TIMER_CLK;

	BYTE temp_byte;

	// PORT A
	#if BTN_USE_PA
		button_enabled_a = BTN_PA;			// Grab enabled pin list
		PAC = PAC & ~button_enabled_a;		// Set control register to input
		PAPH = PAPH | button_enabled_a;		// Set pull high register
		#if BTN_WAKE_SYS					// Set pins to interrupt system, if selected
			temp_byte = 8;
			do
			{
				temp_byte--;
				if (button_enabled_a.0) PADIER = 0;			// PxDIER is write-only. Can't use logic operations
				button_enabled_a = button_enabled_a >> 1;
			} while(temp_byte);
			button_enabled_a = BTN_PA;						// Restore enabled pin list consumed by bit shifts
		#endif
		watchlist_a = 0;					// Reset falling edge watchlist
		active_a = 0;						// Reset active button list
	#endif

	// PORT B
	#if BTN_USE_PB
		button_enabled_b = BTN_PB;
		PBC = PBC & ~button_enabled_b;
		PBPH = PBPH | button_enabled_b;
		#if BTN_WAKE_SYS
			temp_byte = 8;
			do
			{
				temp_byte--;
				if (button_enabled_b.0) PBDIER = 1;
				button_enabled_b = button_enabled_b >> 1;
			} while(temp_byte);
		#endif
		button_enabled_b = BTN_PB;
		watchlist_b = 0;
		active_b = 0;
	#endif

	// PORT C
	#if BTN_USE_PC
		button_enabled_c = BTN_PC;
		PCC = PCC & ~button_enabled_c;
		PCPH = PCPH | button_enabled_c;
		#if BTN_WAKE_SYS
			temp_byte = 8;
			do
			{
				temp_byte--;
				if (button_enabled_c.0) PCDIER = 1;
				button_enabled_c = button_enabled_c >> 1;
			} while(temp_byte);
		#endif
		button_enabled_c = BTN_PC;
		watchlist_c = 0;
		active_c = 0;
	#endif

	INTRQ.BTN_TIMER = 0;					// Ensure that timer interrupt is available
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
		BYTE temp_byte;
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


/* Need to test callback functionality on emulator
void Button_Execute_Callbacks(void)
{
	if (button_module_initialized){

		BYTE temp_byte;

		// PORT A
		#if BTN_USE_PA
			temp_byte = PA; 							// Get button locations 
			button_callbacks_a = button_cb_a;			// Reset CB pointer to first CB address
			while (active_a)							// While there is an unexecuted active pin
			{
				if (active_a.0) *button_callbacks_a();	// Execute CB if current pin is an active button
				if (temp_byte.0) button_callbacks_a++;	// Advance to next CB if current pin is button
				active_a = (active_a >> 1);				// Advance to next pin to check if it is active
				temp_byte = (temp_byte >> 1);			// Advacne to next pin on button list
			}
			button_callbacks_a = button_cb_a;			// Reset CB pointer to first CB address
		#endif

		// PORT B
		#if BTN_USE_PB
			temp_byte = PB; 
			button_callbacks_b = button_cb_b;
			while (active_b)
			{
				if (active_b.0) button_cb_b[1]();
				if (temp_byte.0) button_callbacks_b++;
				active_b = (active_b >> 1);	
				temp_byte = (temp_byte >> 1);
			}
		#endif

		// PORT C
		#if BTN_USE_PC
			temp_byte = PC; 
			button_callbacks_c = button_cb_c;
			while (active_c)
			{
				if (active_c.0) *button_callbacks_c();
				if (temp_byte.0) button_callbacks_c++;	
				active_c = (active_c >> 1);				
				temp_byte = (temp_byte >> 1);			
			}
		#endif


	}
}
*/

// RELEASE
void Button_Release(void)
{
	if (button_module_initialized){

		// Free pins and block button function calls
		BYTE temp_byte;

		// PORT A
		#if BTN_USE_PA
			button_enabled_a = BTN_PA;			// Grab original list of button pins
			PAPH = PAPH & ~button_enabled_a;	// Set buttons to NOPULL		
			#if BTN_WAKE_SYS					// Prevent pins from waking system, if set
				temp_byte = 8;
				do
				{
					temp_byte--;
					if (button_enabled_a.0) PADIER = 0;
					button_enabled_a = button_enabled_a >> 1;
				} while(temp_byte);
			#endif
		#endif

		// PORT B
		#if BTN_USE_PB
			button_enabled_b = BTN_PB;
			PBPH = PBPH & ~button_enabled_b;
			#if BTN_WAKE_SYS
				temp_byte = 8;
				do
				{
					temp_byte--;
					if (button_enabled_b.0) PBDIER = 0;
					button_enabled_b = button_enabled_b >> 1;
				} while(temp_byte);
			#endif
		#endif

		// PORT C
		#if BTN_USE_PC
			button_enabled_c = BTN_PC;
			PCPH = PCPH & ~button_enabled_c;
			#if BTN_WAKE_SYS
			temp_byte = 8;
				do
				{
					temp_byte--;
					if (button_enabled-c.0) PCDIER = 0;
					button_enabled_c = button_enabled_c >> 1;
				} while(temp_byte);
			#endif
		#endif
	
		button_module_initialized = 0;	// Disable functions in module
	}
}