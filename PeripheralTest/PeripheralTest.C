/*
Copyright (c) 2021 Robert R. Puccinelli
*/
#include	"../system_settings.h"
//#include	"../pdk_i2c.h"
//#include	"../pdk_pwm_11b.h"
//#include 	"../pdk_button.h"
//#include 	"../pdk_lcd.h"
//#include	"../pdk_eeprom.h"
#include	"../pdk_common.h"

void	FPPA0 (void)
{
	.ADJUST_IC	SYSCLK=IHRC/4, IHRC=16MHz, VDD=5V;		//	SYSCLK=IHRC/4

	ENGINT;		// Enable global interrupt


	//===================//
	// I2C FEATURE CHECK //
	//===================//
/*
	I2C_Initialize();

	i2c_device = ST7032;	// Identify target device
	I2C_Stream_Write_Start();
	i2c_buffer = 0b11001010;
	I2C_Stream_Write_Byte();
	I2C_Stream_Stop();

	i2c_device = 0b0011100;	// Arbitrary ID
	I2C_Stream_Read_Start();
	I2C_Stream_Read_Byte_Ack();
	I2C_Stream_Read_Byte_NAck();
	I2C_Stream_Stop();

	I2C_Release();
*/


	//=======================//
	// 11b PWM FEATURE CHECK //
	//=======================//

/*
	
// 	USE WITH NO AUTOSOLVER
	prescaler = 1;			// 6-bit  [1, 4, 16, 64]
	scalar = 0;				// 5-bit  [0 : 31]
	counter = 100;			// 11-bit [0 : 2046] in steps of 2
	duty = 60;				// 11-bit [0 : 2047]

//	USE WITH AUTOSOLVER
	f_pwm_target = 14000;	// Desired PWM frequency, pulses per second
	use_pwm_solver = 1; 	// Flag to select PWM solver, if available

 	PWM_11b_Initialize();
	$ PA.4 OUT, HIGH
 	PWM_11b_Set_Parameters();
	$ PA.4 LOW;
 	PWM_11b_Start();
	.delay 4000000;
 	PWM_11b_Stop ();
 	PWM_11b_Release ();
*/


	//======================//
	// BUTTON FEATURE CHECK //
	//======================//

// Enable timer interrupt in the Interrupt function
/*

	Button_Initialize();
	$ PB.1 OUT, HIGH	// Short PB.1 with a button input pin
	Button_Poll();
	$ PB.1 OUT, LOW;

	// Method testing
	Button_Poll();		// Place in while loop for testing
	.delay(100000);		//
	Button_Poll();		// ILRC CLK on ICE is ~35000 kHz
	$ PB.4 OUT, LOW;	// which does impact debounce timer
	Button_Poll();		//
	$ PB.1 OUT, LOW;	//
	Button_Poll();		//
	.delay(100000);		//
	Button_Release();	//

	// Interrupt testing
//	STOPEXE;			// Place in while loop for testing
//	Button_Poll();		// Contact pin to GND to see if wake works
						// Breakpoint on BTN_TIMER Interrupt
/*



	//===================//
	// LCD FEATURE CHECK //
	//===================//

/*
	lcd_device_addr = LCD_DRIVER;

	LCD_Initialize();
	lcd_trx_byte = 0x3;
	LCD_Address_Set();
	lcd_trx_byte = LCD_A;
	LCD_Write_Byte();
	lcd_trx_byte = LCD_Z;
	LCD_Write_Byte();
	lcd_trx_byte = LCD_0;
	LCD_Write_Byte();
	lcd_trx_byte = LCD_9;
	LCD_Write_Byte();
	LCD_MODE_1L();
	LCD_Cursor_Shift_L();
	LCD_Cursor_Shift_L();
	lcd_trx_byte = LCD_9;
	LCD_Write_Byte();
	LCD_Cursor_Shift_R();
	lcd_trx_byte = LCD_9;
	LCD_Write_Byte();
	LCD_Read_Byte();
	LCD_Mode_2L();
	LCD_Check_Addr();
	LCD_Home();
	LCD_Clear();
	LCD_Release();
*/

	//======================//
	// EEPROM FEATURE CHECK //
	//======================//


/*
	BYTE mem_buff[4];
	eeprom_trx_buffer = mem_buff; 

	EEPROM_Initialize();
	mem_buff[0] = 2;          // Num operations
	mem_buff[1] = 1;          // Byte address
	mem_buff[2] = 0b11000011; // operation 1
	mem_buff[3] = 0b01010101; // operation 2
	EEPROM_Write();

	eeprom_trx_buffer = mem_buff;
	mem_buff[1] = 0;          // Read 1B before write
	EEPROM_Read();
	EEPROM_Release();
*/


	//=========================//
	// GENERAL UTILITIES CHECK //
	//=========================//


	byte_divide();



	while (1)
	{
		nop;
	}

}


void	Interrupt (void)
{
	pushaf;


	if (Intrq.TM2)
	{
//		Button_Debounce_Interrupt();
	}

	popaf;
}
