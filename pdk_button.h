/* pdk_button.h

Button peripheral declarations.

Buttons are configured to be digital input, pull high.

ROM Used : 101B / 0x65  -  1 Port
RAM Used :   5B / 0x05  -  1 Port

Each Additional Port
ROM Used + 66B / 0x42
RAM Used +  4B / 0x04
 

*/

EXTERN BYTE		button_enabled_a;
EXTERN BYTE		button_enabled_b;
EXTERN BYTE		button_enabled_c;

EXTERN BYTE 	active_a;
EXTERN BYTE 	active_b;
EXTERN BYTE 	active_c;

/* Callbacks need to be tested on emulator
EXTERN WORD		button_callbacks_a;
EXTERN WORD		button_callbacks_b;
EXTERN WORD		button_callbacks_c;
*/

void	Button_Initialize	    	(void);
void    Button_Poll             	(void);
void	Button_Debounce_Interrupt   (void);
void	Button_Release		    	(void);

