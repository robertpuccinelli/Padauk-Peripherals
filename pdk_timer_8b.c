/* pdk_timer_8b.c
   
8-bit timer utilities.

ROM Consumed : 110B / 0x6E  -  WITHOUT SOLVER
RAM Consumed : 14B  / 0x0E  -  WITHOUT SOLVER

ROM Consumed : 297B / 0x129  -  WITH SOLVER
RAM Consumed :  33B / 0x12   -  WITH SOLVER

	
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

//===========//
// VARIABLES //
//===========//


BYTE timer8_flags = 0;
BIT  timer8_tm2_init : timer8_flags.?;
BIT  timer8_tm3_init : timer8_flags.?;
BIT  timer8_tm2_active : timer8_flags.?;
BIT  timer8_tm3_active : timer8_flags.?;
BIT  timer8_use_param_solver : timer8_flags.?; // Flag to select PWM solver, if available

BYTE	timer8_prescalar = 0;		// 6-bit  [1, 4, 16, 64]
WORD	timer8_scalar = 0;			// 5-bit  [0 : 31]
WORD	timer8_bound = 0;			// 8-bit bound register

#IF SOLVER_OPTION
	STATIC BIT   tm8_use_6b_pwm : timer8_flags.?;
	STATIC BYTE  temp_byte = 0;
	STATIC EWORD tm8_clock_ratio = 0;
	STATIC EWORD tm8_pwm_clk     = 0; // Hz
	EWORD	     tm8_pwm_target  = 0; // Hz
#ENDIF


//==================//
// STATIC FUNCTIONS //
//==================//




/*
static void Timer8_Set_Parameters(void)



static void Solve_PWM_Parameters(void)
{

	if (!tm8_pwm_target) tm8_pwm_target = (tm8_pwm_clk >> 4); // Default : Clk / 16

	// A poor man's division.
	// Find how many PWM_targets fit into PWM_Clk
	// Stop once division has reached or passed 0
	// Takes longer for slower PWMs, up to PWM_Clk cycles
	clock_ratio = 0;
	do
	{
		clock_ratio++;
		tm8_pwm_clk -= tm8_pwm_target;
	} while(  !flag.1 );

	clock_ratio--;

	// Boost low freqs into solvable range
	// Slowest PWM possible is PWM_clk / 4,192,256
	// Going below this results in worst-case solver
	if (tm8_use_6b_pwm) temp_byte = 2016;
	else temp_byte = 8192;

	tm8_prescalar = 0;
	while( (clock_ratio > temp_byte) && (pre_count < 6) )
	{ 
		clock_ratio = clock_ratio >> 2;
		tm8_prescalar += 2;
	}


	// Convert shifts into prescaler value
	if 		(tm8_prescalar == 0)	{tm8_prescalar = 1;}	// 0b00
	else if	(tm8_prescalar == 2)	{tm8_prescalar = 4;}	// 0b01
	else if (tm8_prescalar == 4)	{tm8_prescalar = 16;}	// 0b10
	else if (tm8_prescalar == 6)	{tm8_prescalar = 64;}	// 0b11



	// Solver setup
	scalar = 32;	// 1 above max possible value in PWM formula
	bound = 257; // 2 above max possible value
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
			bound -=2;	// Counter can only be an odd number in PWM formula
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

*/

//===================//
// PROGRAM FUNCTIONS //
//===================//

void Timer2_Stop(void)
{
//	if (timer8_tm2_init) $ TIMER8_2_CTL STOP, TIMER8_2_CTL_SETTINGS;
}


void Timer2_Initialize(void)
{
	#IF TIMER8_USE_2
		if (!timer8_tm2_init)
		{
			Timer2_Stop();
			timer8_tm2_init = 1;
		}
	#ENDIF
}


void Timer2_Start(void)
{
	if (timer8_tm2_init) 
	{
		TIMER8_2_CNT = 0;
//		$ TIMER8_2_CTL TIMER8_2_CLK, TIMER8_2_CTL_SETTINGS;
	}
}


void Timer2_Set_Parameters(void)
{
	if (timer8_tm2_init)
	{
		if (timer8_use_param_solver) 
		{
			if (TIMER8_2_6BIT) tm8_use_6b_pwm = 1;
			else tm8_use_6b_pwm = 0;
			tm8_pwm_clk =  TIMER8_2_HZ;
//			Timer8_Solve_Parameters();
		}

		switch (1)//prescaler)
		{
			case 64 :   TIMER8_2_SCL = TIMER8_2_RES | 0b01100000 | timer8_scalar;
						break;

			case 16 :   TIMER8_2_SCL = TIMER8_2_RES | 0b0100000 | timer8_scalar;
						break;

			case  4 :   TIMER8_2_SCL = TIMER8_2_RES | 0b0100000 | timer8_scalar;
						break;

			default :   TIMER8_2_SCL = TIMER8_2_RES | timer8_scalar;

		}
//		TIMER8_2_SCL = 
//		TIMER8_2_BND = bound;
	}
}


void Timer2_Release(void)
{
	if (timer8_tm2_init) {TIMER8_2_CTL = 0;}
}