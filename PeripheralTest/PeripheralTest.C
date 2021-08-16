/*
Copyright (c) 2021 Robert R. Puccinelli
*/
#include	"../system_settings.h"
//#include	"../pdk_math.h"
//#include	"../pdk_timer_8b.h"
//#include	"../pdk_i2c.h"
//#include	"../pdk_pwm_11b.h"
//#include 	"../pdk_button.h"
//#include 	"../pdk_lcd.h"
//#include	"../pdk_eeprom.h"
#include	"../pdk_stepper.h"

void	FPPA0 (void)
{
	.ADJUST_IC	SYSCLK=IHRC/4, IHRC=16MHz, VDD=5V;		//	SYSCLK=IHRC/4

	ENGINT;		// Enable global interrupt

	//====================//
	// MATH UTILITY CHECK //
	//====================//

/*
	math_dividend = 255;
	math_divisor = 1;
	$ PA.7 OUT, HIGH;
	byte_divide();
	assert
	$ PA.7 LOW;

	math_dividend = 65535;
	math_divisor = 1;
	$ PA.7 HIGH;
	word_divide();
	$ PA.7 LOW;

	math_dividend = 16777215;
	math_divisor = 1;
	$ PA.7 HIGH;
	eword_divide();
	$ PA.7 LOW;

	math_mult_a = 255;
	math_mult_b = 255;
	$ PA.7 HIGH;
	byte_multiply();
	$ PA.7 LOW;

	math_mult_a = 65535;
	math_mult_b = 65535;
	$ PA.7 HIGH;
	word_multiply();
	$ PA.7 LOW;

*/


	//========================//
	// 8b TIMER UTILITY CHECK //
	//========================//

/*
	// Period solver test

	Timer2_Initialize();
	timer8_target_freq = 14000; // 5ms
	timer8_use_solver  = 1;
	$ PA.7 OUT, HIGH;
	Timer2_Set_Parameters();
	$ PA.7 LOW;
	Timer2_Start();
	.delay(1000000)
	Timer2_Stop();
	Timer2_Release();


	// PWM solver test
	
	Timer3_Initialize();
	timer8_target_freq = 20; // 50ms
	timer8_duty_percent = 33;
	timer8_use_solver  = 1;
	$ PA.7 HIGH;
	Timer3_Set_Parameters();
	$ PA.7 LOW;
	Timer3_Start();
	.delay(1000000)
	Timer3_Stop();
	Timer3_Release();

	// Solver-free memory check

	Timer2_Initialize();
	Timer2_Set_Parameters();
	Timer2_Start();
	Timer2_Stop();
	Timer2_Release();

	Timer3_Initialize();
	Timer3_Set_Parameters();
	Timer3_Start();
	Timer3_Stop();
	Timer3_Release();
*/

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
	pwm11_prescalar = 1;			// 6-bit  [1, 4, 16, 64]
	pwm11_scalar = 10;				// 5-bit  [0 : 31]
	pwm11_counter = 100;			// 11-bit [0 : 2046] in steps of 2
	pwm11_duty = 60;				// 11-bit [0 : 2047]

	PWM11_0_Initialize();
	$ PA.5 OUT, HIGH
 	PWM11_0_Set_Parameters();
	$ PA.5 LOW;
 	PWM11_0_Start();
	.delay 800000;
 	PWM11_0_Stop();
 	PWM11_0_Release();

//	USE WITH AUTOSOLVER
	pwm11_target_freq  = 1;	// Desired PWM frequency, pulses per second
	pwm11_duty_percent = 50;
	pwm11_use_solver   = 1; 	// Flag to select PWM solver, if available

 	PWM11_1_Initialize();
	$ PA.5 OUT, HIGH
 	PWM11_1_Set_Parameters();
	$ PA.5 LOW;
 	PWM11_1_Start();
	.delay 800000;
 	PWM11_1_Stop();
 	PWM11_1_Release();

	pwm11_target_freq  = 10;	// Desired PWM frequency, pulses per second
	pwm11_duty_percent = 10;
	pwm11_use_solver   = 1; 	// Flag to select PWM solver, if available

 	PWM11_2_Initialize();
	$ PA.5 OUT, HIGH
 	PWM11_2_Set_Parameters();
	$ PA.5 LOW;
 	PWM11_2_Start();
	.delay 800000;
 	PWM11_2_Stop();
 	PWM11_2_Release();
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


	//=======================//
	// STEPPER FEATURE CHECK //
	//=======================//

	stepper_units_per_rev = 10;
	stepper_steps_per_rev = 1600;

	Stepper_Initialize();
	Stepper_units_per_min = 20;
	Stepper_Set_Vel();
	stepper_dir = 1;
	Stepper_Set_Dir();

	stepper_dist_mode = 0;
	Stepper_Enable();
	Stepper_Start();
	.delay(8000000)
	Stepper_Stop();
	Stepper_Disable();
	Stepper_Release();
	

	Stepper_Initialize();
	stepper_dist_mode = 1;
	stepper_dist_per_run = 5;
	stepper_dir = 0;
	Stepper_Set_Dir();
	Stepper_Enable();
	Stepper_Start();

	if (!stepper_is_moving) // Place in while loop for testing of dist mode
	{
		Stepper_Disable();
		Stepper_Release();
	}


	
	while (1)
	{
		nop;
	}

}


void	Interrupt (void)
{
	pushaf;

/*
	if (Intrq.BTN_INTR)
	{
		Button_Debounce_Interrupt();
	}
*/
	if (Intrq.STEPPER_INTR)
	{
		Stepper_Dist_Mode_Interrupt();
	}

	popaf;
}
