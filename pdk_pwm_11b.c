/* pdk_pwm_11b.c
   
11-bit PWM utilities for ICs with 11-bit PWM functionality.
Specify IC and PWM in system_settings.h
Currently, only one 11-bit PWM can be used. RAM/ROM saving meaure.

ROM Consumed : 74B / 0x4A  -  WITHOUT SOLVER
RAM Consumed : 6B  / 0x06  -  WITHOUT SOLVER

ROM Consumed : 267B / 0x10B  -  WITH SOLVER
RAM Consumed :  25B / 0x19   -  WITH SOLVER

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

	The fastest PWM is (PWM_Clk) and the slowest is (PWM_Clk / 4,192,256).


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
EXTERN BYTE		prescaler;		// 6-bit  [1, 4, 16, 64]
EXTERN WORD		scalar;			// 5-bit  [0 : 31]
EXTERN WORD		counter;		// 11-bit [0 : 2046] in steps of 2
EXTERN BIT		use_pwm_solver; // Flag to select PWM solver, if available
BYTE			temp_byte;		// General purpose byte that is overwritten

// Variables required for solving PWM parameters
#IF SOLVER_OPTION

EXTERN DWORD	f_pwm_target;	// Hz
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
	f_pwm_clk =  SYSTEM_CLOCK;

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
	} while(  !( flag.1 || flag.0 ) );


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
	scalar = 33;	// 1 above max possible value in PWM formula
	counter = 2049; // 2 above max possible value
	product = 0;	// Scaler x Counter
	net = 0;		// f_clk / (f_target * Prescaler) - (Scaler x Counter)
	tolerance = 0;	// Can't solve for prime numbers, etc. Slowly increase tolerance.


	// Find the closest combination of scalar and count registers through brute force.
	// High SYSCLK recommended. Can take ~2M instructions to solve in the worst case.
	do						// Tolerance
	{
		scalar = 32;		// Restore scalar for next parameter scan

		do					// Scalar
		{
			scalar--;		// Decrement scalar for next parameter scan
			counter = 2049;	// Restore counter for next parameter scan
			MULOP = scalar; // Load scalar into the multiplier

			do			// Counter
			{
				counter -= 2;					// Counter can only be an odd number in PWM formula
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
			} while(  (net > temp_byte) && (counter > 1)  );
			

		// Break if net is within tolerance or scalar finished scanning
		} while( (net > temp_byte) && scalar );
			
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
	PWM_DUTY_L 	= PWM_VAL_DUTY_L;
	PWM_DUTY_H 	= PWM_VAL_DUTY_H;
	$ PWM_CTL	PWM_OUTPUT, PWM_CLOCK;
}

void PWM_11b_Set_Parameters(void)
{
	#IF SOLVER_OPTION
	if (use_pwm_solver)	{Solve_PWM_Parameters();}	
	#ENDIF

	if 		(prescaler < 4) 	temp_byte = 0b00;
	else if (prescaler < 16) 	temp_byte = 0b01;
	else if (prescaler < 64)	temp_byte = 0b10;
	else						temp_byte = 0b11;

	PWM_COUNT_L = (counter >> 1) & 0b11;
	PWM_COUNT_H	= (counter >> 3);
	PWM_SCALAR	= (temp_byte << 5) | scalar;
}

void PWM_11b_Start (void)
{
	$ PWM_CTL Enable, Reset;
}

void PWM_11b_Stop (void)
{
	PWM_CTL.7 = 0;
}

void PWM_11b_Release (void)
{
	PWM_11b_Stop();
	$ PWM_CTL Disable;
}