/* pdk_button.h

Button peripheral declarations.

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

EXTERN BYTE		button_enabled_a;
EXTERN BYTE		button_enabled_b;
EXTERN BYTE		button_enabled_c;

EXTERN BYTE 	button_active_a;
EXTERN BYTE 	button_active_b;
EXTERN BYTE 	button_active_c;


void	Button_Initialize	    	(void);
void    Button_Poll             	(void);
void	Button_Debounce_Interrupt   (void);
void	Button_Release		    	(void);

