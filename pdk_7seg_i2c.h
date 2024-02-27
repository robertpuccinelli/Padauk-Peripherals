/* pdk_7seg_i2c.h
   
7-segment display declarations for Padauk microcontrollers. 

With 3B I2C Buffer
ROM Consumed : 116B / 0x74
RAM Consumed :  17B / 0x11

This software is licensed under GPLv3 <http://www.gnu.org/licenses/>.
Any modifications or distributions have to be licensed under GPLv3.
No warranty of any kind and copyright holders cannot be held liable.
Licensees cannot remove copyright notices.

Copyright (c) 2023 Robert R. Puccinelli
*/


//===========//
// VARIABLES //
//===========//

EXTERN BYTE & seg_data_byte;


//===================//
// PROGRAM INTERFACE //
//===================//

void Seg_Initialize(void);
void Seg_First_Digit(void);
void Seg_First_Digit_Command(void);
void Seg_Next_Digit(void);
void Seg_Write(void);
void Seg_Release(void);