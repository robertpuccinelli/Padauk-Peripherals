/* pdk_7seg_i2c.c
   
7-segment display definitions for Padauk microcontrollers. 

With 3B I2C Buffer
ROM Consumed : 116B / 0x74
RAM Consumed    15B / 0x0F

This software is licensed under GPLv3 <http://www.gnu.org/licenses/>.
Any modifications or distributions have to be licensed under GPLv3.
No warranty of any kind and copyright holders cannot be held liable.
Licensees cannot remove copyright notices.

Copyright (c) 2023 Robert R. Puccinelli
*/

#include "system_settings.h"

#IFIDNI SEG_COMM_MODE, I2C 
#include "pdk_i2c.h"

//===========//
// VARIABLES //
//===========//

BYTE & seg_data_byte = i2c_buffer[1];


//===================//
// PROGRAM INTERFACE //
//===================//

void Seg_Initialize(void) {I2C_Initialize();}

void Seg_First_Digit(void) {i2c_dev_addr = SEG_DIG1;}

void Seg_First_Digit_Command(void) {i2c_dev_addr = SEG_SET_PARAM;}

void Seg_Next_Digit(void) {i2c_dev_addr = i2c_dev_addr + SEG_DIG_OFFSET;}

void Seg_Write(void) {I2C_Write_Basic();}

void Seg_Release(void) {I2C_Release();}

#ENDIF // SEG_COMM_MODE