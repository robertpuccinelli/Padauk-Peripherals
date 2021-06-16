/*
Copyright (c) 2021 Robert R. Puccinelli
*/
#include	"../system_settings.h"
#include	"../pdk_i2c.h"
#include	"../pdk_pwm_11b.h"
#include 	"../pdk_button.h"

void	FPPA0 (void)
{
	.ADJUST_IC	SYSCLK=IHRC/4		//	SYSCLK=IHRC/4


	//===================//
	// I2C FEATURE CHECK //
	//===================//

	BYTE buffer[4];			// Initialize communication buffer
	i2c_device = ST7032;	// Identify target device
	i2c_buffer = buffer;	// Link communication buffer
	buffer[0] = 1;			// Number of messages
	buffer[1] = MC24LC00T;	// Message

	I2C_Initialize();
	I2C_Write();
	I2C_Read();
	I2C_Release();


	//=======================//
	// 11b PWM FEATURE CHECK //
	//=======================//


	f_pwm_target = 6500;	// Desired PWM frequency, pulses per second
	prescaler = 64;			// 6-bit  [1, 4, 16, 64]
	scalar = 31;			// 5-bit  [0 : 31]
	counter = 2046;			// 11-bit [0 : 2046] in steps of 2
	duty = 1023;			// 11-bit [0 : 2047]
	use_pwm_solver = 1; 	// Flag to select PWM solver, if available
	
 	PWM_11b_Initialize ();
 	PWM_11b_Release ();
 	PWM_11b_Set_Parameters();
 	PWM_11b_Start ();
 	PWM_11b_Stop ();


	//======================//
	// BUTTON FEATURE CHECK //
	//======================//

	active_a = 0;
	active_b = 0;
	button_enabled_a = 0;
	button_enabled_b = 0;
	Button_Initialize();
	Button_Poll();
	Button_Debounce_Interrupt();	// Call to ensure function is compiled
	Button_Release();


	while (1)
	{
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

	if (Intrq.BTN_TIMER)
	{
		Button_Debounce_Interrupt();
	}
	popaf;
}
