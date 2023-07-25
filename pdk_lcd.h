/* pdk_lcd.h
   
LCD declarations for Padauk microcontrollers. 

I2C, 3B Buffer
ROM Consumed : 261B / 0x105
RAM Consumed :  25B / 0x19


This software is licensed under GPLv3 <http://www.gnu.org/licenses/>.
Any modifications or distributions have to be licensed under GPLv3.
No warranty of any kind and copyright holders cannot be held liable.
Licensees cannot remove copyright notices.

Copyright (c) 2021-2023 Robert R. Puccinelli
*/


//===========//
// VARIABLES //
//===========//

EXTERN BYTE & lcd_trx_byte;

//===================//
// PROGRAM INTERFACE //
//===================//

// LCD Data Control
void LCD_Initialize       (void);
void LCD_Release          (void);
void LCD_Read_Data        (void);
void LCD_Write_Data       (void);

// LCD Function Control
void LCD_Clear            (void);
void LCD_Home             (void);
void LCD_Address_Set      (void);
void LCD_Check_Addr       (void);
void LCD_Mode_1L          (void);
void LCD_Mode_2L          (void);
void LCD_Cursor_Shift_R   (void);
void LCD_Cursor_Shift_L   (void);