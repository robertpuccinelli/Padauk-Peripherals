/* pdk_lcd.h
   
LCD definitions for Padauk microcontrollers.

ROM Consumed : 221B / 0xDD
RAM Consumed :  11B / 0x0B


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

BYTE	lcd_device_addr; 	// Device address. Program sets target address w/enum above.
BYTE	lcd_trx_byte;
BYTE	lcd_flags = 0;
BIT		lcd_detected : lcd_flags.?;
BIT		lcd_module_initialized : lcd_flags.?;


LCD_Init_Delay =>   LCD_INIT_D
LCD_Pwr_Delay  =>   LCD_PWR_D
LCD_Wait_Delay =>   LCD_WAIT_D

//==================//
// STATIC FUNCTIONS //
//==================//

void	LCD_Write_Command(void)
{                
	#ifidni LCD_COMM_MODE, I2C
		i2c_device = lcd_device_addr;
		I2C_Stream_Write_Start();
		i2c_buffer = LCD_COMMAND_MODE;
		I2C_Stream_Write_Byte();
		i2c_buffer = lcd_trx_byte;
		I2C_Stream_Write_Byte();
		I2C_Stream_Stop();
	#endif
}


void	LCD_Read_Command(void)
{
	#ifidni LCD_COMM_MODE, I2C
		i2c_device = lcd_device_addr;
		I2C_Stream_Read_Start();
		i2c_buffer = LCD_COMMAND_MODE;
		I2C_Stream_Write_Byte();
		I2C_Stream_Read_Byte_NAck();
		lcd_trx_byte = i2c_buffer;
		I2C_Stream_Stop();
		if (i2c_slave_ack_bit == 1) lcd_detected = 0;
		else lcd_detected = 1;
	#endif
}

void	LCD_Write_Data(void)
{
	#ifidni LCD_COMM_MODE, I2C
		i2c_device = lcd_device_addr;
		I2C_Stream_Write_Start();
		i2c_buffer = LCD_DATA_MODE;
		I2C_Stream_Write_Byte();
		i2c_buffer = lcd_trx_byte;
		I2C_Stream_Write_Byte();
		I2C_Stream_Stop();
	#endif
}

void	LCD_Read_Data(void)
{                
	#ifidni LCD_COMM_MODE, I2C
		i2c_device = lcd_device_addr;
		I2C_Stream_Read_Start();
		i2c_buffer = LCD_DATA_MODE;
		I2C_Stream_Write_Byte();
		I2C_Stream_Read_Byte_Nack();
		lcd_trx_byte = i2c_buffer;
		I2C_Stream_Stop();
	#endif
}


void	LCD_Check_Busy (void)
{
	LCD_Read_Command();
	lcd_trx_byte = (lcd_trx_byte & LCD_BUSY_MASK);
}


void	LCD_Delay_While_Busy (void)
{
	#ifdifi %LCD_DRIVER, %ST7032
		do LCD_Check_Busy();
		while (lcd_trx_byte && lcd_detected);
	#else
		.delay LCD_Wait_Delay;
	#endif
}


//===================//
// PROGRAM INTERFACE //
//===================//


void	LCD_Read_Byte	(void)
{
	if ( lcd_module_initialized)
	{
		LCD_Delay_While_Busy();
		LCD_Read_Data();
	}
}


void	LCD_Write_Byte	(void)
{
	if ( lcd_module_initialized)
	{
		LCD_Delay_While_Busy();
		LCD_Write_Data();
	}
}


void	LCD_Clear		(void)
{
	if ( lcd_module_initialized)
	{
		LCD_Delay_While_Busy();
		lcd_trx_byte = (LCD_CLEAR_F);
		LCD_Write_Command();
	}
}


void	LCD_Home	(void)
{
	if ( lcd_module_initialized)
	{
		LCD_Delay_While_Busy();
		lcd_trx_byte = (LCD_HOME_F);
		LCD_Write_Command();
	}
}


void	LCD_Address_Set	(void)
{
	if ( lcd_module_initialized)
	{
		LCD_Delay_While_Busy();
		lcd_trx_byte = (lcd_trx_byte | LCD_SET_DDRAM_ADDR);
		LCD_Write_Command();
	}
}


void	LCD_Check_Addr (void)
{
	if ( lcd_module_initialized)
	{
		LCD_Read_Command();
		lcd_trx_byte = (lcd_trx_byte & LCD_ADDR_MASK);
	}
}


void	LCD_Mode_1L		(void)
{
	if ( lcd_module_initialized)
	{
		LCD_Delay_While_Busy();
		lcd_trx_byte = LCD_1L_SETTINGS;
		LCD_Write_Command();
	}
}


void	LCD_Mode_2L		(void)
{
	if ( lcd_module_initialized)
	{
		LCD_Delay_While_Busy();
		lcd_trx_byte = LCD_2L_SETTINGS;
		LCD_Write_Command();
	}
}


void	LCD_Cursor_Shift_R (void)
{
	if ( lcd_module_initialized)
	{
		LCD_Delay_While_Busy();
		lcd_trx_byte = (LCD_SHIFT_F | LCD_SHIFT_CURSOR_CTL | LCD_SHIFT_RIGHT);
		LCD_Write_Command();
	}
}


void	LCD_Cursor_Shift_L (void)
{
	if ( lcd_module_initialized)
	{
		LCD_Delay_While_Busy();
		lcd_trx_byte = (LCD_SHIFT_F | LCD_SHIFT_CURSOR_CTL | LCD_SHIFT_LEFT);
		LCD_Write_Command();
	}
}


void	LCD_Initialize	(void)
{
	if ( !lcd_module_initialized)
	{
		.delay(LCD_Init_Delay);

		#ifidni LCD_COMM_MODE, I2C
			I2C_Initialize();
		#endif

		#ifidni %LCD_DRIVER, %ST7032
			lcd_trx_byte = LCD_INIT_FUNC1;
			LCD_Write_Command();
			.delay(LCD_Wait_Delay);

			lcd_trx_byte = LCD_INIT_FUNC2;
			LCD_Write_Command();
			.delay(LCD_Wait_Delay);
	
			lcd_trx_byte = LCD_INIT_BIAS_OSC;
			LCD_Write_Command();
			.delay(LCD_Wait_Delay);

			lcd_trx_byte = LCD_INIT_CONTRASTL;
			LCD_Write_Command();
			.delay(LCD_Wait_Delay);
	
			lcd_trx_byte = LCD_INIT_PWR_ICON_CNTRSTH;
			LCD_Write_Command();
			.delay(LCD_Wait_Delay);

			lcd_trx_byte = LCD_INIT_FOLLOWER;
			LCD_Write_Command();
			.delay(LCD_Pwr_Delay);

			lcd_trx_byte = (LCD_DISP_F | LCD_DISP_ON);
			LCD_Write_Command();
			.delay(LCD_Wait_Delay);

			lcd_trx_byte = (LCD_CLEAR_F);
			LCD_Write_Command();
			.delay(LCD_Init_Delay)

			lcd_trx_byte = (LCD_ENTRY_F | LCD_ENTRY_INC_DDRAM);
			LCD_Write_Command();
			.delay(LCD_Wait_Delay)

		#endif

		lcd_module_initialized = 1;
		LCD_Read_Command();

		if (!lcd_detected)
		{
			lcd_module_initialized = 0;
			I2C_Release();
		}

	}
}


void	LCD_Release		(void)
{
	if ( lcd_module_initialized)
	{
		I2C_Release();
		lcd_module_initialized = 0;
	}
}