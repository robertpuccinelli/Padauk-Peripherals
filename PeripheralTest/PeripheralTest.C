/*
Copyright (c) 2021 Robert R. Puccinelli
*/
#include	"../system_settings.h"
//#include	"../pdk_i2c.h"
//#include	"../pdk_pwm_11b.h"
//#include 	"../pdk_button.h"
#include 	"../pdk_lcd.h"
void	FPPA0 (void)
{
	.ADJUST_IC	SYSCLK=IHRC/4, IHRC=16MHz, VDD=5V;		//	SYSCLK=IHRC/4


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
/*
	active_a = 0;
	active_b = 0;
	button_enabled_a = 0;
	button_enabled_b = 0;
	Button_Initialize();
	Button_Poll();
	Button_Debounce_Interrupt();	// Call to ensure function is compiled
	Button_Release();

*/
/*
	#define dead_zone_R 2
	#define dead_zone_F 3

	Byte duty1  =  60;
	Byte _duty = 100 - duty1;
	PWMG0DTL	= 0x00;
	PWMG0DTH = _duty;
	PWMG0CUBL = 0x00;
	PWMG0CUBH = 100;
	PWMG1DTL = 0x00;
	PWMG1DTH = _duty -dead_zone_F;
	PWMG1CUBL = 0x00;
	PWMG1CUBH = 100;
	$ PWMG0C Enable,Inverse,PB5,SYSCLK;
	$ PWMG0S INTR_AT_DUTY,/1,/1;
	.delay dead_zone_R;
	$ PWMG1C Enable, PA4, SYSCLK;
	$ PWMG1S INTR_AT_DUTY, /1, /1;
*/
	while (1)
	{
		nop;
	}

}


void	Interrupt (void)
{
	pushaf;
/*
	if (Intrq.T16)
	{	//	T16 Trig
		//	User can add code
		Intrq.T16	=	0;
		//...
	}
*/
/*
	if (Intrq.BTN_TIMER)
	{
		Button_Debounce_Interrupt();
	}
*/
	popaf;
}
