/* pdk_eeprom.c
   
EEPROM definitions for Padauk microcontrollers.

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

#include "system_settings.h"

#IF PERIPH_EEPROM
#include "pdk_i2c.h"

//===========//
// VARIABLES //
//===========//

BYTE eeprom_trx_buffer;
BYTE eeprom_device_addr = EEPROM_DRIVER;
BYTE eeprom_flags = 0;
BIT	 eeprom_module_initialized : eeprom_flags.?;
BIT  eeprom_busy : eeprom_flags.?;
BYTE count = 0;


//==================//
// STATIC FUNCTIONS //
//==================//

void EEPROM_Write_Enable (void)
{
	#ifdifi EEPROM_WRITE_CTL, NONE
		$ EEPROM_WRITE_CTL Low;
	#endif
}


void EEPROM_Write_Disable (void)
{
	#ifdifi EEPROM_WRITE_CTL, NONE
		$ EEPROM_WRITE_CTL HIGH;
	#endif
}



void	EEPROM_Check_Busy (void)
{
	#ifidni EEPROM_COMM_MODE, I2C
		i2c_device = eeprom_device_addr;
		I2C_Stream_Write_Start();
		I2C_Stream_Stop();
		eeprom_busy = 0;
		if (i2c_slave_ack_bit) {eeprom_busy = 1;}
	#endif
}


void	EEPROM_Delay_While_Busy (void)
{
	do EEPROM_Check_Busy();
	while (eeprom_busy);
}

//===================//
// PROGRAM INTERFACE //
//===================//

void EEPROM_Initialize (void)
{
	if ( !eeprom_module_initialized)
	{
		I2C_Initialize();
		eeprom_module_initialized = 1;
		#ifdifi EEPROM_WRITE_CTL, NONE
			$ EEPROM_WRITE_CTL Out, High;
		#endif
	}
}


void EEPROM_Release (void)
{
	if (eeprom_module_initialized)
	{
		I2C_Release();
		eeprom_module_initialized = 0;
		#ifdifi EEPROM_WRITE_CTL, NONE
			$ EEPROM_WRITE_CTL In;
		#endif
	}
}


void EEPROM_Read (void)
{
	if (eeprom_module_initialized)
	{
		count = *eeprom_trx_buffer++;
		if (count <= EEPROM_PAGE_SIZE)
		{
			EEPROM_Delay_While_Busy();
			#ifidni EEPROM_COMM_MODE, I2C

				// Set read address
				i2c_buffer[0] = eeprom_device_addr;
				i2c_buffer[1] = *eeprom_trx_buffer++;
				I2C_Stream_Write_Byte();
				I2C_Stream_Stop();

				// Start reading
				I2C_Stream_Read_Start();
				while (--count) 
				{
					i2c_buffer = eeprom_trx_buffer++;
					I2C_Stream_Read_Byte_Ack(); 
				}
				i2c_buffer = *eeprom_trx_buffer;
				I2C_Stream_Read_Byte_NAck();
				I2C_Stream_Stop();
			#endif
		}
	}
}


void EEPROM_Write (void)
{
	if (eeprom_module_initialized)
	{
		count = *eeprom_trx_buffer++;
		if (count <= EEPROM_PAGE_SIZE)
		{
			EEPROM_Delay_While_Busy();
			EEPROM_Write_Enable();
			#ifidni EEPROM_COMM_MODE, I2C
				i2c_device = eeprom_device_addr;
				I2C_Stream_Write_Start();
				do 
				{
					i2c_buffer = *eeprom_trx_buffer++;
					I2C_Stream_Write_Byte(); 
				} while(--count);
				i2c_buffer = *eeprom_trx_buffer;
				I2C_Stream_Write_Byte();
				I2C_Stream_Stop();
			#endif
			EEPROM_Write_Disable();
		}
	}
}

#ENDIF // PERIPH_EEPROM
