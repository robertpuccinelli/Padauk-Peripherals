/* pdk_pwm_11b.c
   
11-bit PWM utilities for ICs with 11-bit PWM functionality.
Specify IC and PWM in system_settings.h
Currently, only one 11-bit PWM can be used. RAM/ROM saving meaure.

ROM Consumed : 110B / 0x6E  -  WITHOUT SOLVER
RAM Consumed : 14B  / 0x0E  -  WITHOUT SOLVER

ROM Consumed : 297B / 0x129  -  WITH SOLVER
RAM Consumed :  33B / 0x12   -  WITH SOLVER


DOCUMENTATION ERROR:

	During testing, the observed PWM was consistently 2x faster than the PWM predicted with the
	equation in section 8.5.3 of the PMS132 manual. This was also observed with the demo code
	in section 8.5.4, where the equation predicted 1.25 kHz, but the device ran at 2.5 kHz. To 
	compensate for this unexpected event, the PWM_CLK is increased by 2x in the autosolver function.
	End users should be aware of this discrepancy before use.
	

NOTE: 

	ICs with an 8x8 multiplier have the option to solve for register settings
	that result in an output frequency closest to the target frequency. However,
	in a worst-case scenario it can take ~2M + 2xPWM_Clk cycles to iterate through all options.
	Worst-case is a very fast PWM clock and a very slow target PWM frequency, < slowest possible PWM.
	2nd worst case is very fast PWM frequency (~PWM clock). ~2M cycles
	
	In the PMS132 datasheet frequency can be solved with the following equation:

	PWM_Frequency = PWM_Clk / [ Prescaler x (Scaler + 1) x (Counter + 1) ]

			Prescaler 	: [1, 4, 16, 64]
			Scaler		: [0 : 31]
			Counter		: [0 : 2046], in steps of 2

	The fastest PWM is (PWM_Clk) and the slowest is (2 x PWM_Clk / 4,192,256).

	Autosolver Testing:
	PWM CLK = SYSCLK = 4000000

	Target  = 200000
	Result  = 200000
	Error   = 0.000%
	Time	= 331 ms
	
	Target  = 20000
	Result  = 20000
	Error   = 0.000%
	Time    = 329 ms

	Target  = 17561
	Result  = 17582.4
	Error   = 0.122%
	Time    = 260 ms	
	
	Target  = 14000
	Result  = 14010.5
	Error   = 0.075%
	Time    = 241 ms
	
	Target  = 6500
	Result  = 6504.07
	Error   = 0.063%
	Time    = 236 ms

	Target  = 2000
	Result  = 2000
	Error   = 0.000%
	Time    = 321 ms
	
	Target  = 1000
	Result  = 999.87
	Error   = 0.0125%
	Time    = 494 ms

	Target  = 100
	Result  = 100
	Error   = 0.000%
	Time    = 518 ms

	Target  = 10
	Result  = 9.9996
	Error   = 0.004%
	Time    = 3613 ms

	Target  = 1
	Result  = 2.101
	Error   = 99.988%
	Time    = 37160 ms


This software is licensed under GPLv3 <http://www.gnu.org/licenses/>.
Any modifications or distributions have to be licensed under GPLv3.
No warranty of any kind and copyright holders cannot be held liable.
Licensees cannot remove copyright notices.

Copyright (c) 2021 Robert R. Puccinelli
*/

#include "system_settings.h"


// 11-bit PWM Required
#IFNDEF HAS_11B_PWM
	.error IC with 11-bit PWM required. Add IC entry to system_settings.h if new IC target.
#ENDIF


// Multiplier optional
#IFNDEF HAS_MULTIPLIER
	.echo NOTE: IC IC_TARGET does NOT have a multiplier. Cannot use PWM parameter solving function!
#ENDIF


//===========//
// VARIABLES //
//===========//

// PWM_Frequency = PWM_Clk / [ Prescaler x (Scaler + 1) x (Counter + 1) ]
BYTE		prescaler;		// 6-bit  [1, 4, 16, 64]
WORD		scalar;			// 5-bit  [0 : 31]
WORD		counter;		// 11-bit [0 : 2046] in steps of 2
WORD		duty = 0xFFFF;	// 11-bit [0 : 2047], if 0xFFFF, duty is set to 50%
BYTE		temp_byte;		// General purpose byte that is overwritten
BYTE		pwm_flags = 0;
BIT			pwm_module_initialized : pwm_flags.?;


// Variables required for solving PWM parameters
#IF SOLVER_OPTION

BIT				use_pwm_solver : pwm_flags.?; // Flag to select PWM solver, if available
DWORD			f_pwm_target;	// Hz
DWORD			f_pwm_clk;		// Hz
DWORD			clock_ratio;	// PWM_Clk / PWM_Target, pulses per second
BYTE 			pre_count;

WORD			product;
WORD			net;
BYTE			tolerance;

#ENDIF


//==================//
// STATIC FUNCTIONS //
//=================-//

#IF SOLVER_OPTION

static void Solve_PWM_Parameters(void)
{
	// Fastest PWM is PWM_Clk;
	// Slowest PWM is PWM_Clk / 4,192,256
	// F_Target above or below leads to worst case performance
	f_pwm_clk =  PWM_CLOCK_HZ << 1;

	if (!f_pwm_target) f_pwm_target=1;

	// A poor man's division.
	// Find how many PWM_targets fit into PWM_Clk
	// Stop once division has reached or passed 0
	// Takes longer for slower PWMs, up to PWM_Clk cycles
	clock_ratio = 0;
	do
	{
		clock_ratio++;
		f_pwm_clk -= f_pwm_target;
	} while(  !flag.1 );

	clock_ratio--;

	// Boost low freqs into solvable range
	// Slowest PWM possible is PWM_clk / 4,192,256
	// Going below this results in worst-case solver
	pre_count = 0;
	while( (clock_ratio > 65504) && (pre_count < 6) )
	{ 
		clock_ratio = clock_ratio >> 2;
		pre_count += 2;
	}


	// Convert shifts into prescaler value
	if 		(pre_count == 0)	{prescaler = 1;}	// 0b00
	else if	(pre_count == 2)	{prescaler = 4;}	// 0b01
	else if (pre_count == 4)	{prescaler = 16;}	// 0b10
	else if (pre_count == 6)	{prescaler = 64;}	// 0b11


	// Solver setup
	scalar = 32;	// 1 above max possible value in PWM formula
	counter = 2049; // 2 above max possible value
	product = 0;	// Scaler x Counter
	net = 0;		// f_clk / (f_target * Prescaler) - (Scaler x Counter)
	tolerance = 0;	// Can't solve for prime numbers, etc. Slowly increase tolerance.


	// Find the closest combination of scalar and count registers through brute force.
	// High SYSCLK recommended. Can take ~2M instructions to solve in the worst case.
	do						// Tolerance
	{
	counter = 2049;	// Restore counter for next parameter scan

		do					// Scalar
		{
			counter -=2;	// Counter can only be an odd number in PWM formula
			scalar = 33;	// Restore scalar for next parameter scan	

			do			// Counter
			{
				scalar--;
				MULOP = scalar; // Load scalar into the multiplier
				A = counter$0;					// Multiply lower byte of counter with scalar 
				mul;
				product$0 = A;					// Store the product
				product$1 = MULRH;	
				A = counter$1;					// Multiply upper byte of counter with scalar
				mul;
				product$1 += A;					// Add product to stored high byte, MULRH is always 0 here
				net = clock_ratio - product;	// How close to zero?


				// Compiler can only handle simple comparisons.
				if (flag.1) { net = -net; }
				temp_byte = tolerance + 1;


			// Break if net is within tolerance or counter finished scanning
			} while(  (net > temp_byte) && (scalar > 1)  );
			

		// Break if net is within tolerance or scalar finished scanning
		} while( (net >= temp_byte) && (counter > 1) );
			
		tolerance++;	// Increment tolerance for next parameter scan

	// Break if net	is within tolerance or tolerance exceeds limit
	} while ( (net > temp_byte) && (tolerance < 65) );	// Max tolerance: 64 -> (max scalar x space between count values)

	
	// If scanning failed, use the slowest PWM
	// Consider using a slower clock if you need a very slow PWM
	if (tolerance > 65 && net) 
	{
		scalar = 32;
		counter = 2047;
	}


	scalar--;	// Decrement scalar to compensate for +1 in formula
	counter--;	// Decrement scalar to compensate for +1 in formula

}

#ENDIF //SOLVER_OPTION


//===================//
// PROGRAM FUNCTIONS //
//===================//

void PWM_11b_Initialize (void)
{
	if (! pwm_module_initialized)
	{
       $ PWM_CTL	PWM_OUTPUT, PWM_CLOCK;
	   pwm_module_initialized = 1;
	}
}

void PWM_11b_Set_Parameters(void)
{
	if (pwm_module_initialized)
	{
		#IF SOLVER_OPTION
		if (use_pwm_solver)	{Solve_PWM_Parameters();}	
		#ENDIF

		if 		(prescaler < 4) 	temp_byte = 0b00;
		else if (prescaler < 16) 	temp_byte = 0b01;
		else if (prescaler < 64)	temp_byte = 0b10;
		else						temp_byte = 0b11;

		if (duty == 0xFFFF) duty = counter >> 1;

		duty <<= 5;
		PWM_DUTY_L 	= duty$0;
		PWM_DUTY_H 	= duty$1;

		counter <<= 5;
		PWM_COUNT_L = counter$0;
		PWM_COUNT_H	= counter$1;

		PWM_SCALAR	= (temp_byte << 5) | scalar;
	}
}


void PWM_11b_Start (void)
{
	if (pwm_module_initialized)
	{
		PWM_CTL = PWM_CTL | PWM_RST | PWM_ENABLE;
	}
}


void PWM_11b_Stop (void)
{
	if (pwm_module_initialized)
	{
		PWM_CTL = PWM_CTL & ~PWM_ENABLE;
	}
}


void PWM_11b_Release (void)
{
	if (pwm_module_initialized)
	{
		PWM_11b_Stop();
		PWM_CTL = PWM_DISABLE;
		pwm_module_initialized = 0;
	}
}