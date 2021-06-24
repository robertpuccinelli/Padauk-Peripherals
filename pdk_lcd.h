/* pdk_lcd.h
   
LCD declarations for Padauk microcontrollers..

!!!OUT OF DATE!!!
ROM Consumed : 109B / 0x6D
RAM Consumed :  14B / 0x0E


This software is licensed under GPLv3 <http://www.gnu.org/licenses/>.
Any modifications or distributions have to be licensed under GPLv3.
No warranty of any kind and copyright holders cannot be held liable.
Licensees cannot remove copyright notices.

Copyright (c) 2021 Robert R. Puccinelli
*/


//===========//
// VARIABLES //
//===========//

EXTERN BYTE lcd_device_addr;
EXTERN BYTE lcd_trx_byte;
EXTERN BYTE lcd_detected;


//===================//
// PROGRAM INTERFACE //
//===================//

// LCD Data Control
void LCD_Initialize       (void);
void LCD_Release          (void);
void LCD_Read_Byte        (void);
void LCD_Write_Byte       (void);

// LCD Function Control
void LCD_Clear            (void);
void LCD_Home             (void);
void LCD_Address_Set      (void);
void LCD_Check_Addr       (void);
void LCD_Mode_1L          (void);
void LCD_Mode_2L          (void);
void LCD_Cursor_Shift_R   (void);
void LCD_Cursor_Shift_L   (void);