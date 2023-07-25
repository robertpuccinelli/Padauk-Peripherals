/* pdk_lcd.h
   
LCD definitions for Padauk microcontrollers.
Data bytes are transiently stored in the I2C buffer when I2C is used.

I2C, 3B Buffer
ROM Consumed : 261B / 0x105
RAM Consumed :  25B / 0x19


This software is licensed under GPLv3 <http://www.gnu.org/licenses/>.
Any modifications or distributions have to be licensed under GPLv3.
No warranty of any kind and copyright holders cannot be held liable.
Licensees cannot remove copyright notices.

Copyright (c) 2021 Robert R. Puccinelli
*/

#include "system_settings.h"

#IF PERIPH_LCD
#include "pdk_i2c.h"

//===========//
// VARIABLES //
//===========//

BYTE & lcd_trx_byte = i2c_buffer[1];


LCD_Init_Delay =>   LCD_INIT_D
LCD_Pwr_Delay  =>   LCD_PWR_D
LCD_Wait_Delay =>   LCD_WAIT_D

//==================//
// STATIC FUNCTIONS //
//==================//

void	LCD_Write_Command(void)
{                
	#ifidni LCD_COMM_MODE, I2C
		i2c_dev_addr = LCD_DRIVER;
		i2c_reg_addr = LCD_COMMAND_MODE;
		I2C_Write_Random();
	#endif
}


void	LCD_Read_Command(void)
{
	#ifidni LCD_COMM_MODE, I2C
		i2c_dev_addr = LCD_DRIVER;
		i2c_reg_addr = LCD_COMMAND_MODE;
		I2C_Read_Random();
	#endif
}

void	LCD_Check_Busy (void)
{
	LCD_Read_Command();
	lcd_trx_byte = (lcd_trx_byte & LCD_BUSY_MASK);
}


void	LCD_Delay_While_Busy (void)
{
	#ifdifi %LCD_DRIVER, ST7032
		do LCD_Check_Busy();
		while (lcd_trx_byte);
	#else
		.delay LCD_Wait_Delay;
	#endif
}


//===================//
// PROGRAM INTERFACE //
//===================//

void	LCD_Write_Data(void)
{
	#ifidni LCD_COMM_MODE, I2C
		i2c_dev_addr = LCD_DRIVER;
		i2c_reg_addr = LCD_DATA_MODE;
		I2C_Write_Random();
	#endif
}

void	LCD_Read_Data(void)
{                
	#ifidni LCD_COMM_MODE, I2C
		i2c_dev_addr = LCD_DRIVER;
		i2c_reg_addr = LCD_DATA_MODE;
		I2C_Read_Random();
	#endif
}



void	LCD_Clear		(void)
{
	lcd_trx_byte = (LCD_CLEAR_F);
	LCD_Delay_While_Busy();
	LCD_Write_Command();
	.delay LCD_Init_Delay;
}


void	LCD_Home	(void)
{
	lcd_trx_byte = (LCD_HOME_F);
	LCD_Delay_While_Busy();
	LCD_Write_Command();
}


void	LCD_Address_Set	(void)
{
	lcd_trx_byte = (lcd_trx_byte | LCD_SET_DDRAM_ADDR);
	LCD_Delay_While_Busy();
	LCD_Write_Command();
}


void	LCD_Check_Addr (void)
{
	LCD_Delay_While_Busy();
	LCD_Read_Command();
	lcd_trx_byte = (lcd_trx_byte & LCD_ADDR_MASK);
}


void	LCD_Mode_1L		(void)
{
	lcd_trx_byte = LCD_1L_SETTINGS;
	LCD_Delay_While_Busy();
	LCD_Write_Command();
}


void	LCD_Mode_2L		(void)
{
	lcd_trx_byte = LCD_2L_SETTINGS;
	LCD_Delay_While_Busy();
	LCD_Write_Command();
}


void	LCD_Cursor_Shift_R (void)
{
	lcd_trx_byte = (LCD_SHIFT_F | LCD_SHIFT_CURSOR_CTL | LCD_SHIFT_RIGHT);
	LCD_Delay_While_Busy();
	LCD_Write_Command();
}


void	LCD_Cursor_Shift_L (void)
{
	lcd_trx_byte = (LCD_SHIFT_F | LCD_SHIFT_CURSOR_CTL | LCD_SHIFT_LEFT);
	LCD_Delay_While_Busy();
	LCD_Write_Command();
}


void	LCD_Initialize	(void)
{
	.delay(LCD_Init_Delay);

	#ifidni LCD_COMM_MODE, I2C
		I2C_Initialize();
	#endif

	#ifidni %LCD_DRIVER, ST7032
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
}


void	LCD_Release		(void)
{
	#ifidni LCD_COMM_MODE, I2C
		I2C_Release();
	#endif
}

#ENDIF // PERIPH_LCD