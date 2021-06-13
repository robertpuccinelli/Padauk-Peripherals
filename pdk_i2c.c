/* pdk_i2c.c
   
I2C utilities based off of the PADAUK IDE v0.91 Code Generator. 
Define PERIPH_I2C in system_settings.h

ROM Consumed : 109B / 0x6D
RAM Consumed :  14B / 0x0E  -  WITH 4B FOR COMMUNICATION BUFFER


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

enum	{	I2C_WR_CMD	=	0b0, 
			I2C_RD_CMD	=	0b1	};

EXTERN BYTE i2c_device;		// Device address. Known addresses in header.
EXTERN WORD i2c_buffer;		// Pointer to program communication buffer. 
							// [NumMessagess, Message1, Message2, ...]

Delay_High		=>	T_High ?  (((System_Clock / FPPA_Duty) / (1000000000 / T_High)) + 1) : 0;
Delay_Low		=>	T_Low ?   (((System_Clock / FPPA_Duty) / (1000000000 / T_Low)) + 1) : 0;
Delay_Start		=>	T_Start ? (((System_Clock / FPPA_Duty) / (1000000000 / T_Start)) + 1) : 0;
Delay_Stop		=>	T_Stop ?  (((System_Clock / FPPA_Duty) / (1000000000 / T_Stop)) + 1) : 0;
Delay_Buf		=>	T_Buf ?   (((System_Clock / FPPA_Duty) / (1000000000 / T_Buf)) + 1) : 0;


Easy_Delay	macro	val, cmp
	#IF	val > cmp
		.delay	val - cmp;
	#ENDIF
	endm


//==============//
// I2C PROTOCOL //
//==============//

static	void	I2C_Start (void)
{
	$ I2C_SDA	Low;
	Easy_Delay	(Delay_Start, 1);
	$ I2C_SCL	Low;

}

static 	void	I2C_Tx_Bit (void)
{
	sl A;
	swapc I2C_SDA;
	Easy_Delay (Delay_Low, 2);
	$ I2C_SCL High;
	Easy_Delay (Delay_High, 1)
	$ I2C_SCL Low;
}


static	void	I2C_Tx_ACC (void)
{
	.REPEAT 8
		I2C_Tx_Bit();
	.ENDM
}

static	void	I2C_Rx_Bit (void)
{
	Easy_Delay (Delay_Low, 0);
	$ I2C_SCL High;
	swapc I2C_SDA;
	slc A;
	Easy_Delay (Delay_High, 3)
	$ I2C_SCL Low;
}

static	void	I2C_Rx_Acc (void)
{
	$ I2C_SDA In;
	.REPEAT 8
		I2C_Rx_Bit();
	.ENDM
}

static	void	I2C_Listen_Ack (void)
{
	$ I2C_SDA In;
	Easy_Delay (Delay_Low, 1);
	$ I2C_SCL High;
	swapc I2C_SDA;				// Carry bit is high if ACK fails. For future use.
	$ I2C_SDA Out, Low;
	Easy_Delay (Delay_High, 3)
	$ I2C_SCL Low;
}

static	void	I2C_Stop (void)
{
	$ I2C_SDA	Low;
	Easy_Delay (Delay_Low, 1);

	$ I2C_SCL	High;
	Easy_Delay (Delay_Stop, 1);

	$ I2C_SDA	High;		
	Easy_Delay (Delay_Buf, 1);
}


//===================//
// PROGRAM FUNCTIONS //
//===================//

void I2C_Initialize	(void)
{
	$ I2C_SCL	Out, High;
	$ I2C_SDA	In, Pull;
	$ I2C_SDA	Out, High;
}

void I2C_Release (void)
{
	$ I2C_SDA	In, NoPull;
	$ I2C_SCL	In, NoPull;
}

void	I2C_Write (void)
{
	BYTE count;
	count = *i2c_buffer;

	A	=	(i2c_device << 1) | I2C_WR_CMD;
	I2C_Start();
	I2C_Tx_ACC();
	I2C_Listen_Ack();

	do
	{
		i2c_buffer++;
		A = *i2c_buffer;
		I2C_Tx_ACC();
		I2C_Listen_Ack();
	}	while(--count);

	I2C_Stop();
}

void	I2C_Read (void)
{
	BYTE count;
	count = *i2c_buffer;

	A	=	(i2c_device << 1) |	I2C_RD_CMD;	
	I2C_Start();
	I2C_Tx_ACC();
	I2C_Listen_Ack();

	do{
		i2c_buffer++;
		I2C_Rx_ACC();
		I2C_Listen_Ack();
		*i2c_buffer = A;
	} while(--count);

	I2C_Stop();
}
