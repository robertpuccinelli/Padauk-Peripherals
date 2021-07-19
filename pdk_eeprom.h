/* pdk_eeprom.h
   
EEPROM declarations for Padauk microcontrollers.


ROM Consumed : 96B / 0x60
RAM Consumed : 15B / 0x0F  -  USING 4B BUFFER


USAGE NOTE:

	The buffer is formatted [2B] + [NB]. The first byte is used to identify the
	number of data bytes to process and the second byte is the EEPROM device ID.
	The N bytes after are used for reading or writing data. The first byte specifies
	how many of these bytes will be processed.

	
This software is licensed under GPLv3 <http://www.gnu.org/licenses/>.
Any modifications or distributions have to be licensed under GPLv3.
No warranty of any kind and copyright holders cannot be held liable.
Licensees cannot remove copyright notices.

Copyright (c) 2021 Robert R. Puccinelli
*/


//===========//
// VARIABLES //
//===========//

EXTERN WORD eeprom_trx_buffer;		// Pointer to array : [NumOps, Addr, Op1, Op2, ..., OpM]


//===================//
// PROGRAM INTERFACE //
//===================//

void EEPROM_Initialize (void);
void EEPROM_Release    (void);
void EEPROM_Read       (void);
void EEPROM_Write      (void);