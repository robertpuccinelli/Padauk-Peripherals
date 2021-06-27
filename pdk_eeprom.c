/* pdk_eeprom.c
   
EEPROM definitions for Padauk microcontrollers.

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
#include "pdk_i2c.h"

//===========//
// VARIABLES //
//===========//

WORD eeprom_trx_buffer;
BYTE eeprom_device_addr;
BIT	 eeprom_module_initialized;
BYTE count;

eeprom_device_addr = EEPROM_DRIVER;
eeprom_module_initialized = 0;
count = 0;

//====================//
// HARDWARE INTERFACE //
//====================//

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
			#ifidni EEPROM_COMM_MODE, I2C

				// Set read address
				i2c_device = eeprom_device_addr;
				I2C_Stream_Write_Start();
				i2c_buffer = eeprom_trx_buffer++;
				I2C_Stream_Write_Byte();
				I2C_Stream_Stop();

				// Start reading
				I2C_Stream_Read_Start();
				while (--count) 
				{
					i2c_buffer = eeprom_trx_buffer++;
					I2C_Stream_Read_Byte_Ack(); 
				}
				i2c_buffer = eeprom_trx_buffer;
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
			EEPROM_Write_Enable();
			#ifidni EEPROM_COMM_MODE, I2C
				i2c_device = eeprom_device_addr;
				I2C_Stream_Write_Start();
				do 
				{
					i2c_buffer = eeprom_trx_buffer++;
					I2C_Stream_Write_Byte(); 
				} while(--count);
				i2c_buffer = eeprom_trx_buffer;
				I2C_Stream_Write_Byte();
				I2C_Stream_Stop();
			#endif
			EEPROM_Write_Disable();
		}
	}
}
