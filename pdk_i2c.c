/* pdk_i2c.c
   
I2C utilities based off of the PADAUK IDE v0.91 Code Generator. 
Define PERIPH_I2C in system_settings.h

In the communication protocol provided here, the MSB is Tx/Rx first.


!!!OUT OF DATE!!!
ROM Consumed : 109B / 0x6D
RAM Consumed :  14B / 0x0E


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

BYTE i2c_device;		      // 7-bit slave address.
WORD i2c_buffer;		      // Pointer to Tx/Rx byte. 
BIT  i2c_slave_ack_bit;       // Slave acknowledge bit


BIT  i2c_module_initialized;  // Module function blocking flag
BYTE i2c_num_initializations; // Number of initializations 


// Default states
i2c_module_initialized   = 0;
i2c_num_initializations  = 0;
i2c_slave_ack_bit        = 1; 


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


//====================//
// HARDWARE INTERFACE //
//====================//

static void I2C_Start (void)
{
	$ I2C_SDA	Low;
	Easy_Delay	(Delay_Start, 1);
	$ I2C_SCL	Low;

}


static void I2C_Tx_Bit (void)
{
	sl A;
	swapc I2C_SDA;
	Easy_Delay (Delay_Low, 2);
	$ I2C_SCL High;
	Easy_Delay (Delay_High, 1)
	$ I2C_SCL Low;
}


static void I2C_Tx_ACC (void)
{
	.REPEAT 8
		I2C_Tx_Bit();
	.ENDM
}


static void I2C_Rx_Bit (void)
{
	Easy_Delay (Delay_Low, 0);
	$ I2C_SCL High;
	swapc I2C_SDA;
	slc A;
	Easy_Delay (Delay_High, 3)
	$ I2C_SCL Low;
}


static void I2C_Rx_Acc (void)
{
	$ I2C_SDA In;
	.REPEAT 8
		I2C_Rx_Bit();
	.ENDM
}


static void I2C_Read (void)
{
	I2C_Rx_ACC();
	*i2c_buffer = A;
}


static void I2C_Provide_Ack (void)
{
	$ I2C_SDA Out, Low;
	Easy_Delay (Delay_Low, 2);
	$ I2C_SCL High;
	Easy_Delay (Delay_High, 1);
	$ I2C_SCL Low;
}


static void I2C_Provide_NAck (void)
{
	$ I2C_SDA Out, High;
	Easy_Delay (Delay_Low, 2);
	$ I2C_SCL High;
	Easy_Delay (Delay_High, 1);
	$ I2C_SCL Low;
}


static void I2C_Listen_Ack (void)
{
	$ I2C_SDA In;
	Easy_Delay (Delay_Low, 1);
	$ I2C_SCL High;
	swapc I2C_SDA;
	slc A;
	i2c_slave_ack_bit = 0b1;			
	$ I2C_SDA Out;
	Easy_Delay (Delay_High, 4);
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


//===================//
// PROGRAM INTERFACE //
//===================//

void I2C_Initialize (void)
{
	if (! i2c_num_initializations)  // If not yet initialized 
	{
		$ I2C_SCL	Out, High;      // Clock pin set output high
		$ I2C_SDA	In, Pull;       // Set data input pull high register
		$ I2C_SDA	Out, High;      // Set data input to output high
		i2c_module_initialized = 1; // Enable I2C functions
	}
	i2c_num_initializations++;      // Count number of initializations
}


void I2C_Release (void)
{
	if (i2c_module_initialized)
	{
		i2c_num_initializations--;      // Count remaining initializations
		if (! i2c_num_initializations)   // If none remaining
		{
			$ I2C_SDA	In, NoPull;     // Set data to low power
			$ I2C_SCL	In, NoPull;     // Set clock to low power
			i2c_module_initialized = 0; // Disable I2C functions
		}
	}
}


void I2C_Stream_Write_Byte (void)
{
	if (i2c_module_initialized)
	{
		A = *i2c_buffer;  // Transfer buffer contents to ACC
		I2C_Tx_ACC();     // Transmit individual bits
		I2C_Listen_Ack(); // Listen for slave ack
	}
}


void I2C_Stream_Write_Start (void)
{
	if (i2c_module_initialized)
	{
		I2C_Start();                                  // I2C start condition
		*i2c_buffer = (i2c_device << 1) | I2C_WR_CMD; // Transfer device addr + WR bit to buffer
		I2C_Stream_Write_Byte();                      // Transmit buffer
	}
}


void I2C_Stream_Read_Start (void)
{
	if (i2c_module_initialized)
	{
		I2C_Start();                                  // I2C start condition
		*i2c_buffer = (i2c_device << 1) | I2C_RD_CMD; // Transfer device addr + RD bit to buffer
		I2C_Stream_Write_Byte();                      // Transmit buffer
	}
}


void I2C_Stream_Read_Byte_Ack (void)
{
	if (i2c_module_initialized)
	{
		I2C_Read();        // Listen for byte
		I2C_Provide_Ack(); // Provide master ack
	}
}


void I2C_Stream_Read_Byte_NAck (void)
{
	if (i2c_module_initialized)
	{
		I2C_Read();         // Listen for byte
		I2C_Provide_NAck(); // Provide master nack
	}
}


void I2C_Stream_Stop (void)
{
	if (i2c_module_initialized) I2C_Stop(); // I2C stop condition
}
