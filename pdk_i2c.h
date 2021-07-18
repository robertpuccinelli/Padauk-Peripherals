/* pdk_i2c.h
   
I2C declarations based off of the PADAUK IDE v0.91 Code Generator. 
Define PERIPH_I2C in system_settings.h

In the communication protocol provided here, the MSB is Tx/Rx first.
Timings are configured for 100kHz with system_settings.h. Higher may
be achievable, but it is currently untested.

ROM Consumed : 197B / 0xC5
RAM Consumed :  12B / 0x0C


This software is licensed under GPLv3 <http://www.gnu.org/licenses/>.
Any modifications or distributions have to be licensed under GPLv3.
No warranty of any kind and copyright holders cannot be held liable.
Licensees cannot remove copyright notices.

Copyright (c) 2021 Robert R. Puccinelli
*/


//===========//
// VARIABLES //
//===========//

EXTERN BYTE i2c_device;	       // Device address.
EXTERN BYTE i2c_buffer;	       // Pointer to Tx/Rx byte.
EXTERN BIT  i2c_slack_ack_bit; // Slave acknowledge bit.


//===================//
// PROGRAM INTERFACE //
//===================//

void I2C_Initialize            (void);
void I2C_Release               (void);
void I2C_Stream_Write_Start    (void);
void I2C_Stream_Read_Start     (void);
void I2C_Stream_Write_Byte     (void);
void I2C_Stream_Read_Byte_Ack  (void);
void I2C_Stream_Read_Byte_NAck (void);
void I2C_Stream_Stop           (void);
