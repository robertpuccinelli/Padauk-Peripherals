/* pdk_eeprom.h
   
EEPROM declarations for Padauk microcontrollers.

!!!OUT OF DATE!!!
ROM Consumed : 109B / 0x6D
RAM Consumed :  14B / 0x0E


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