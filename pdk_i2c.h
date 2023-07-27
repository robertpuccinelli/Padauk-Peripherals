/* pdk_i2c.h
   
I2C declarations based off of the PADAUK IDE v0.91 Code Generator. 
Define PERIPH_I2C in system_settings.h

In the communication protocol provided here, the MSB is Tx/Rx first.
Timings are configured for 100kHz with system_settings.h. Higher may
be achievable, but it is currently untested.

Basic with 2B buffer
ROM Consumed : 132B / 0x84
RAM Consumed :  12B / 0x0C

Basic and Standard with 3B buffer
ROM Consumed : 156B / 0x9C
RAM Consumed :  13B / 0x0D

Only Standard with 3B buffer
ROM Consumed : 139B / 0x8B
RAM Consumed :  13B / 0x0D

All functions with 5B buffer
ROM Consumed : 210B / 0xD2
RAM Consumed :  15B / 0x0F

NOTES:
	Devices can be configured for handling I2C data in different ways,
	so set I2C_BUFF_SIZE accordingly in system_settings.h. When using
	sequential read/write calls, set the 2nd to last byte to be the 
	number of bytes to process in the read/write action.
	
	Buffer size examples:
		[2] - Dev Addr, Data (Basic device, see Freetronics Relay8)
		[3] - Dev Addr, Data, Reg Addr (Standard single byte action)
		[5] - Dev Addr, Data1, Data2, Length, Reg Addr (Multi byte action)

This software is licensed under GPLv3 <http://www.gnu.org/licenses/>.
Any modifications or distributions have to be licensed under GPLv3.
No warranty of any kind and copyright holders cannot be held liable.
Licensees cannot remove copyright notices.

Copyright (c) 2023 Robert R. Puccinelli
*/


//===========//
// VARIABLES //
//===========//

EXTERN WORD i2c_p_data;			// Pointer to current data byte
EXTERN BYTE i2c_buffer[I2C_BUFF_SIZE];
EXTERN BIT i2c_slave_ack_bit;

EXTERN BYTE & i2c_dev_addr;			// Element of array for all ops
EXTERN BYTE & i2c_reg_addr;	// Element of array for random ops
EXTERN BYTE & i2c_read_len;	// Sequential read parameter
EXTERN BYTE & i2c_write_len;	// Sequential write parameter

//===================//
// PROGRAM INTERFACE //
//===================//

void I2C_Initialize					(void);
void I2C_Release					(void);
void I2C_Write_Basic				(void);
void I2C_Write_Random				(void);
void I2C_Write_Random_Sequential	(void);
void I2C_Read_Basic					(void);
void I2C_Read_Random				(void);
void I2C_Read_Random_Sequential		(void);
void I2C_Data_Start					(void);	// Return data buffer pointer to beginning
void I2C_Data_Next					(void);	// Increment data buffer pointer