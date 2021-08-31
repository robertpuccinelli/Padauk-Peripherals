/* pdk_pwm_11b.c
   
11-bit PWM utilities for ICs with 11-bit PWM functionality.
Specify IC and PWM in system_settings.h

ROM Consumed : 281B / 0x119  -  Generators 1/2/3 - SOLVER
RAM Consumed : 18B  / 0x12   -  Generators 1/2/3 - SOLVER

ROM Consumed : 648B / 0x288  -  Generators 1/2/3 + SOLVER
RAM Consumed :  39B / 0x27   -  Generators 1/2/3 + SOLVER


DOCUMENTATION ERROR:

	During testing, the observed PWM was consistently 2x faster than the PWM predicted with the
	equation in section 8.5.3 of the PMS132 manual. This was also observed with the demo code
	in section 8.5.4, where the equation predicted 1.25 kHz, but the device ran at 2.5 kHz. To 
	compensate for this unexpected event, the clock_ratio is increased by 2x in the solver function.
	

NOTE: 

	16.7 MHz is currently the highest supported frequency due to the use of EWORDs.
	DO NOT USE IHRC x 2 PWM CLOCK!

	This version of the 11b PWM utilizes pdk_math utilities since they provide more stable
	computation times and a 100x performance improvement at the expense of memory and slightly
	inferior error rates. Error rate might be reduced by multiplying Clk x2 instead of clock
	ratio. This would require DWORD division, which currently does not exist.
		
	
	In the PMS132 datasheet frequency can be solved with the following equation:

	PWM_Frequency = PWM_Clk / [ Prescaler x (Scaler + 1) x (Counter + 1) ]

			Prescaler 	: [1, 4, 16, 64]
			Scaler		: [0 : 31]
			Counter		: [0 : 2046], in steps of 2

	Autosolver Testing:
	PWM CLK = SYSCLK = 4000000 Hz
	Time to solve    = 3 ms

	Target  = 200000
	Result  = 200000
	Error   = 0.000%
	
	Target  = 20000
	Result  = 20000
	Error   = 0.000%

	Target  = 17561
	Result  = 17621
	Error   = 0.342%
	
	Target  = 14000
	Result  = 14035
	Error   = 0.251%
	
	Target  = 6500
	Result  = 6504
	Error   = 0.063%

	Target  = 2000
	Result  = 2000
	Error   = 0.000%
	
	Target  = 1000
	Result  = 1000
	Error   = 0.0125%

	Target  = 100
	Result  = 100
	Error   = 0.000%

	Target  = 10
	Result  = 10
	Error   = 0.004%

	Target  = 1      // Out of range
	Result  = 1.908
	Error   = 90.98%


This software is licensed under GPLv3 <http://www.gnu.org/licenses/>.
Any modifications or distributions have to be licensed under GPLv3.
No warranty of any kind and copyright holders cannot be held liable.
Licensees cannot remove copyright notices.

Copyright (c) 2021 Robert R. Puccinelli
*/

#IFIDNI PERIPH_PWM_11B, 1

#include "system_settings.h"
#include "pdk_math.h"

// 11-bit PWM Required
#IFNDEF HAS_11B_PWM
	.error IC with 11-bit PWM required. Add IC entry to system_settings.h if new IC target.
#ENDIF


//===========//
// VARIABLES //
//===========//


BYTE pwm11_flags     = 0;
BYTE pwm11_prescalar = 0; // 6-bit  [1, 4, 16, 64]
WORD pwm11_scalar    = 0; // 5-bit  [0 : 31]
WORD pwm11_duty      = 0; // 11-bit [0 : 2047]
WORD pwm11_counter   = 0; // 11-bit [0 : 2046] in steps of 2

BIT  pwm11_pwm0_init  : pwm11_flags.?;
BIT  pwm11_pwm1_init  : pwm11_flags.?;
BIT  pwm11_pwm2_init  : pwm11_flags.?;
BIT  pwm11_use_solver : pwm11_flags.?; // Flag to select PWM solver, if available


#IF PWM_SOLVER_ENABLE

	STATIC EWORD  pwm11_clock_ratio;	// PWM_Clk / PWM_Target, pulses per second
	STATIC EWORD  pwm11_clk_freq;		// Hz
	STATIC BYTE  &pwm11_tolerance    = pwm11_clk_freq$0;
	STATIC BYTE  &pwm11_scalar_temp  = pwm11_clk_freq$1;
	BYTE         &pwm11_duty_percent = pwm11_duty$0;
	EWORD        &pwm11_target_freq  = pwm11_clock_ratio$0;	// Hz

#ENDIF


//==================//
// STATIC FUNCTIONS //
//=================-//

#IF PWM_SOLVER_ENABLE

static void Solve_PWM_Parameters(void)
{
	if (!pwm11_target_freq) pwm11_target_freq = (pwm11_clk_freq >> 4);

	// Find ratio between clk freq and desired pwm freq
	math_dividend = pwm11_clk_freq;
	math_divisor  = pwm11_target_freq;
	eword_divide();
	pwm11_clock_ratio = math_quotient;


	// Documentation error correction
	pwm11_clock_ratio <<= 1;


	// Boost low freqs into solvable range
	pwm11_prescalar = 0;
	while( (pwm11_clock_ratio > 65504) && (pwm11_prescalar < 6) )
	{ 
		pwm11_clock_ratio >>= 2;
		pwm11_prescalar += 2;
	}


	// Convert shifts into prescaler value
	if 		(pwm11_prescalar == 0)	{pwm11_prescalar = 1;}	// 0b00
	else if	(pwm11_prescalar == 2)	{pwm11_prescalar = 4;}	// 0b01
	else if (pwm11_prescalar == 4)	{pwm11_prescalar = 16;}	// 0b10
	else if (pwm11_prescalar == 6)	{pwm11_prescalar = 64;}	// 0b11


	// Scan through scalar values to find optimal solution
	pwm11_scalar_temp = 1; 
	pwm11_tolerance = 32;
	do
	{
		math_dividend = pwm11_clock_ratio;
		math_divisor  = pwm11_scalar_temp;
		word_divide();

		// To update, counter must be odd, less than 2^11, and tolerance must improve
		if ((math_remainder < pwm11_tolerance) && math_quotient.0 && (math_quotient < 2048))
		{
			pwm11_tolerance = math_remainder;
			pwm11_counter = math_quotient;
			pwm11_scalar = pwm11_scalar_temp;
		}

		pwm11_scalar_temp++;

	} while (pwm11_scalar_temp < 33);

	if ((pwm11_counter == 0) || (pwm11_scalar == 0))
	{
		pwm11_counter = 2047;
		pwm11_scalar = 32;
	}

	pwm11_scalar--;	// Decrement scalar to compensate for +1 in formula
	pwm11_counter--;	// Decrement scalar to compensate for +1 in formula
}


static void Solve_Duty(void)
{
	if (pwm11_duty_percent > 100) pwm11_duty_percent = 50;

	math_mult_a = pwm11_duty_percent;
	math_mult_b = pwm11_counter + 1;
	word_multiply();

	math_dividend = math_product;
	math_divisor = 100;
	eword_divide();

	pwm11_duty = math_quotient;
	math_remainder -= 50;

	if (math_remainder > 50) pwm11_duty.0 = 1;
	else pwm11_duty.0 = 0;
}
#ENDIF //PWM_SOLVER_ENABLE


static void Convert_Prescalar(void)
{
	if 		(pwm11_prescalar == 4) 	pwm11_prescalar = 0b0100000;
	else if (pwm11_prescalar == 16) pwm11_prescalar = 0b1000000;
	else if (pwm11_prescalar == 64)	pwm11_prescalar = 0b1100000;
	else							pwm11_prescalar = 0b0000000;
}


//===================//
// PROGRAM FUNCTIONS //
//===================//


// PWM 0

void PWM11_0_Stop (void)
{ 
	if (pwm11_pwm0_init) $ PWM_0_CTL PWM_0_POL, PWM_0_OUTPUT, PWM_0_CLOCK; 
}


void PWM11_0_Start (void)
{ 
	if (pwm11_pwm0_init) $ PWM_0_CTL PWM_ENABLE, PWM_RESET, PWM_0_POL, PWM_0_OUTPUT, PWM_0_CLOCK; 
}


void PWM11_0_Set_Parameters(void)
{
	if (pwm11_pwm0_init)
	{
		#IF PWM_SOLVER_ENABLE
			if (pwm11_use_solver)
			{
				pwm11_clk_freq = PWM_0_CLK_HZ;
				Solve_PWM_Parameters();	
				Solve_Duty();
			}
		#ENDIF

		Convert_Prescalar();

		PWM_0_SCALAR = pwm11_prescalar | pwm11_scalar | PWM_0_INT;

		pwm11_duty <<= 5;
		PWM_0_DUTY_L = pwm11_duty$0;
		PWM_0_DUTY_H = pwm11_duty$1;

		pwm11_counter <<= 5;
		PWM_0_COUNT_L = pwm11_counter$0;
		PWM_0_COUNT_H = pwm11_counter$1;
	}
}


void PWM11_0_Initialize (void)
{
	if (!pwm11_pwm0_init && PWM_USE_G0)
	{
		PWM11_0_Stop();
		pwm11_pwm0_init = 1;
	}
}


void PWM11_0_Release (void)
{
	if (pwm11_pwm0_init)
	{
		PWM11_0_Stop();
		pwm11_pwm0_init = 0;
	}
}



// PWM 1

void PWM11_1_Stop (void)
{ if (pwm11_pwm1_init) $ PWM_1_CTL PWM_1_POL, PWM_1_OUTPUT, PWM_1_CLOCK; }


void PWM11_1_Start (void)
{ if (pwm11_pwm1_init) $ PWM_1_CTL PWM_ENABLE, PWM_RESET, PWM_1_POL, PWM_1_OUTPUT, PWM_1_CLOCK; }


void PWM11_1_Set_Parameters(void)
{
	if (pwm11_pwm1_init)
	{
		#IF PWM_SOLVER_ENABLE
			if (pwm11_use_solver)
			{
				pwm11_clk_freq = PWM_1_CLK_HZ;
				Solve_PWM_Parameters();	
				Solve_Duty();
			}
		#ENDIF

		Convert_Prescalar();

		PWM_1_SCALAR = pwm11_prescalar | pwm11_scalar | PWM_1_INT;

		pwm11_duty <<= 5;
		PWM_1_DUTY_L = pwm11_duty$0;
		PWM_1_DUTY_H = pwm11_duty$1;

		pwm11_counter <<= 5;
		PWM_1_COUNT_L = pwm11_counter$0;
		PWM_1_COUNT_H = pwm11_counter$1;
	}
}


void PWM11_1_Initialize (void)
{
	if (!pwm11_pwm1_init && PWM_USE_G1)
	{
		PWM11_1_Stop();
		pwm11_pwm1_init = 1;
	}
}


void PWM11_1_Release (void)
{
	if (pwm11_pwm1_init)
	{
		PWM11_1_Stop();
		pwm11_pwm1_init = 0;
	}
}



// PWM 2

void PWM11_2_Stop (void)
{ if (pwm11_pwm2_init) $ PWM_2_CTL PWM_2_POL, PWM_2_OUTPUT, PWM_2_CLOCK; }


void PWM11_2_Start (void)
{ if (pwm11_pwm2_init) $ PWM_2_CTL PWM_ENABLE, PWM_RESET, PWM_2_POL, PWM_2_OUTPUT, PWM_2_CLOCK; }


void PWM11_2_Set_Parameters(void)
{
	if (pwm11_pwm2_init)
	{
		#IF PWM_SOLVER_ENABLE
			if (pwm11_use_solver)
			{
				pwm11_clk_freq = PWM_2_CLK_HZ;
				Solve_PWM_Parameters();	
				Solve_Duty();
			}
		#ENDIF

		Convert_Prescalar();

		PWM_2_SCALAR = pwm11_prescalar | pwm11_scalar | PWM_2_INT;

		pwm11_duty <<= 5;
		PWM_2_DUTY_L = pwm11_duty$0;
		PWM_2_DUTY_H = pwm11_duty$1;

		pwm11_counter <<= 5;
		PWM_2_COUNT_L = pwm11_counter$0;
		PWM_2_COUNT_H = pwm11_counter$1;
	}
}


void PWM11_2_Initialize (void)
{
	if (!pwm11_pwm2_init && PWM_USE_G2)
	{
		PWM11_2_Stop();
		pwm11_pwm2_init = 1;
	}
}


void PWM11_2_Release (void)
{
	if (pwm11_pwm2_init)
	{
		PWM11_2_Stop();
		pwm11_pwm1_init = 0;
	}
}

#ENDIF \\PERIPH_PWM_11B
