﻿/* pdk_i2c.c
   
I2C utilities based off of the PADAUK IDE v0.91 Code Generator. 
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

#include "system_settings.h"

#IF PERIPH_I2C

//======================//
// VARIABLES AND MACROS //
//======================//

BYTE i2c_buffer[I2C_BUFF_SIZE] = 0;						// I2C buffer array
WORD i2c_p_data;
i2c_p_data = &i2c_buffer[0];	// Pointer to current data byte

// Aliases for elements in buffer array, does not use memory

// !! CANNOT USE ALIASES IN PROGRAM IF DEFINED IN .C FILE !! //
// DEFINITIONS LEFT HERE TO HELP TRANSLATE SOURCE CODE //

BYTE & i2c_dev_addr				= i2c_buffer[0];			// Element of array for all ops
BYTE & i2c_reg_addr				= i2c_buffer[I2C_BUFF_END];	// Element of array for random ops
BYTE & i2c_read_len				= i2c_buffer[I2C_BUFF_LEN];	// Sequential read parameter
BYTE & i2c_write_len			= i2c_read_len;				// Sequential write parameter
BYTE & i2c_data_0				= i2c_buffer[1];			// Element of array for all ops 
BYTE & i2c_data_n				= i2c_buffer[I2C_DATA_END];	// Element of array for sequential r/w

BYTE i2c_flags = 0;
BIT  i2c_slave_ack_bit : i2c_flags.?;       // Slave acknowledge bit


// Delay cycles
Delay_High  =>  I2C_D_HIGH;
Delay_Low   =>  I2C_D_LOW;
Delay_Start =>  I2C_D_START;
Delay_Stop  =>  I2C_D_STOP;
Delay_Buf   =>  I2C_D_BUF;


// Offset target delay
Easy_Delay	macro	val, cmp
	#IF	val > cmp
		.delay	val - cmp;
	#ENDIF
	endm

/*
#IFZ _SYS (OP:SWAPC IO.n)            // Determine if there is an instruction of swapc io.n
    swapc  macro   iob             // use macro instead of swapc instruction
        set0	flag.c;
        t0sn	iob;
        set1	flag.c;
    endm
#ENDIF
*/

//===================//
// PRIVATE FUNCTIONS //
//===================//

static void I2C_Start (void)
{
	$ I2C_SDA	Low;
	Easy_Delay	(Delay_Start, 1);
	$ I2C_SCL	Low;

}


static void I2C_Rx_ACC (void)
{
	BYTE TEMP = 8;
	$ I2C_SDA In;
	while(TEMP--)
	{
		Easy_Delay (Delay_Low, 4)
		$ I2C_SCL High;
		swapc I2C_SDA;
		slc A;
		Easy_Delay (Delay_High, 2)
		$ I2C_SCL Low;
	}
}


static void I2C_Tx_Ack (void)
{
	$ I2C_SDA Out, Low;
	Easy_Delay (Delay_Low, 2);
	$ I2C_SCL High;
	Easy_Delay (Delay_High, 1);
	$ I2C_SCL Low;
}


static void I2C_Tx_NAck (void)
{
	$ I2C_SDA Out, High;
	Easy_Delay (Delay_Low, 2);
	$ I2C_SCL High;
	Easy_Delay (Delay_High, 1);
	$ I2C_SCL Low;
}


static void I2C_Stop (void)
{
	$ I2C_SDA	Low;
	Easy_Delay (Delay_Low, 1);

	$ I2C_SCL	High;
	Easy_Delay (Delay_Stop, 1);

	$ I2C_SDA	High;		
	Easy_Delay (Delay_Buf, 1);
}


static void I2C_Write_ACC (void)
{
	// Tx ACC - Send byte in ACC
	BYTE TEMP = 8;
	while(TEMP--)
	{
		sl A;
		swapc I2C_SDA;
		Easy_Delay (Delay_Low, 8)
		$ I2C_SCL High;
		Easy_Delay (Delay_High, 0)
		$ I2C_SCL Low;
	}

	// Rx Ack - Listen for slave ack
	$ I2C_SDA In;
	Easy_Delay (Delay_Low, 2);
	$ I2C_SCL High;
	i2c_slave_ack_bit = 0;
	if (I2C_SDA) {i2c_slave_ack_bit = 1;}
	Easy_Delay (Delay_High, 3);
	$ I2C_SCL Low;
	$ I2C_SDA Out;
}


static void I2C_Start_Reg_Write (void)
{
	I2C_Start();
	A = (i2c_buffer[0] << 1) | I2C_WR_CMD; 		  // Transfer device addr + WR bit to buffer
	I2C_Write_ACC();
	A = i2c_buffer[I2C_BUFF_END];
	I2C_Write_ACC();
}

static void I2C_Start_Read (void)
{
	I2C_Start();
	A = (i2c_buffer[0] << 1) | I2C_RD_CMD; 		  // Transfer device addr + WR bit to buffer
	I2C_Write_ACC();
	I2C_Rx_ACC();
}


//===================//
// PROGRAM INTERFACE //
//===================//

void I2C_Initialize (void)
{
	$ I2C_SDA	In, Pull;       // Set data input pull high register
	$ I2C_SDA	Out, High;      // Set data input to output high
	$ I2C_SCL	Out, High;      // Clock pin set output high
}


void I2C_Release (void)
{
	$ I2C_SCL	In, NoPull;     // Set clock to low power
	$ I2C_SDA	In, NoPull;     // Set data to low power
}

void I2C_Data_Start (void)
{
	i2c_p_data = &i2c_data_0;
}

void I2C_Data_Next (void)
{
	if (i2c_p_data != &i2c_data_n) {i2c_p_data++;}
}

void I2C_Write_Basic (void)
{
	I2C_Data_Start();
	I2C_Start();
	A = (i2c_buffer[0] << 1) | I2C_WR_CMD; 		  // Transfer device addr + WR bit to buffer
	I2C_Write_ACC();
	A = *i2c_p_data;
	I2C_Write_ACC();
	I2C_Stop();
}

void I2C_Write_Random (void)
{
	I2C_Data_Start();
	I2C_Start_Reg_Write();
	A = *i2c_p_data;
	I2C_Write_ACC();
	I2C_Stop();
}

void I2C_Write_Random_Sequential (void)
{
	I2C_Data_Start();
	I2C_Start_Reg_Write();
	while(i2c_buffer[I2C_BUFF_LEN]--) 
	{
		A = *i2c_p_data;
		I2C_Write_ACC();
		I2C_Data_Next();
	}
	I2C_Stop();
	I2C_Data_Start();
}

void I2C_Read_Basic (void)
{
	I2C_Data_Start();
	I2C_Start_Read();
	*i2c_p_data = A;
	I2C_Tx_NAck();
	I2C_Stop();
}

void I2C_Read_Random (void)
{
	I2C_Data_Start();
	I2C_Start_Reg_Write();
	I2C_Start_Read();
	*i2c_p_data = A;
	I2C_Tx_NAck();
	I2C_Stop();
}

void I2C_Read_Random_Sequential (void)
{
//	BYTE & i2c_read_len	= i2c_buffer[I2C_BUFF_LEN];
	I2C_Data_Start();
	I2C_Start_Reg_Write();
	I2C_Start_Read();
	while(i2c_buffer[I2C_BUFF_LEN]--)
	{
		I2C_Tx_Ack();
		I2C_Rx_ACC();
		*i2c_p_data = A;
		I2C_Data_Next();
	}
	I2C_Tx_NAck();
	I2C_Stop();
	I2C_Data_Start();
}

#ENDIF // PERIPH_I2C