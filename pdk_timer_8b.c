/* pdk_timer_8b.c
   
8-bit timer utilities.

ROM Consumed : 110B / 0x6E  -  WITHOUT SOLVER
RAM Consumed : 14B  / 0x0E  -  WITHOUT SOLVER

ROM Consumed : 297B / 0x129  -  WITH SOLVER
RAM Consumed :  33B / 0x12   -  WITH SOLVER

	
DOCUMENTATION ERROR:

	The PMS132 datasheet claims that the bound register for a 8b timer
	can range from 0 - 255. That was not observed to be the case for
	a timer in period mode. When bound = 0 in period mode, the output
	will not change states.

This software is licensed under GPLv3 <http://www.gnu.org/licenses/>.
Any modifications or distributions have to be licensed under GPLv3.
No warranty of any kind and copyright holders cannot be held liable.
Licensees cannot remove copyright notices.

Copyright (c) 2021 Robert R. Puccinelli
*/

#include "system_settings.h"
#include "pdk_math.h"


//===========//
// VARIABLES //
//===========//

BYTE timer8_flags     = 0;
BYTE timer8_prescalar = 0; // 6-bit  [1, 4, 16, 64]
BYTE timer8_scalar    = 0; // 5-bit  [0 : 31]
BYTE timer8_bound     = 0; // 8-bit bound register

BIT  timer8_tm2_init   : timer8_flags.?;
BIT  timer8_tm3_init   : timer8_flags.?;
BIT  timer8_tm2_active : timer8_flags.?;
BIT  timer8_tm3_active : timer8_flags.?;
BIT  timer8_use_solver : timer8_flags.?; // Flag to select solver, if available
BIT  timer8_use_6b_pwm : timer8_flags.?;

#IF TIMER8_SOLVER_ENABLE

	STATIC EWORD  timer8_clock_ratio  = 0;
	STATIC EWORD  timer8_pwm_clk      = 0;
	STATIC BYTE  &timer8_tolerance    = timer8_bound$0;
	BYTE         &timer8_duty_percent = timer8_bound$0;       // Integer, [0 : 100]
	EWORD        &timer8_target_freq  = timer8_clock_ratio$0; // Hz

#ENDIF


//==================//
// STATIC FUNCTIONS //
//==================//

#IF TIMER8_SOLVER_ENABLE


static void Timer8_Solve_Duty(void)
{
	// 50% duty cycle if no target given
	if (!timer8_duty_percent || (timer8_duty_percent > 100)) timer8_duty_percent = 50;

	// Multiply clock by duty percent
	math_mult_a = timer8_pwm_clk;
	math_mult_b = timer8_duty_percent;
	byte_multiply();

	// Convert bound to integer
	math_dividend = math_product;
	math_divisor = 100;
	word_divide();

	timer8_bound = math_quotient$0 - 1;
}


static void Timer8_Solve_PWM(void)
{
	if (!timer8_target_freq) timer8_target_freq = (timer8_pwm_clk >> 4); // Default : Clk / 16


	// PWM_CLK / BIT_DEPTH => PWM CYCLES per SECOND
	math_dividend = timer8_pwm_clk;
	if (timer8_use_6b_pwm) math_divisor = 64;
	else math_divisor = 256;
	eword_divide();


	// PWM CYCLES per SECOND / TARGET CYCLES per SECOND => PRESCALAR x (SCALAR + 1)
	math_dividend  = math_quotient;
	math_divisor = timer8_target_freq;
	eword_divide();
	timer8_clock_ratio = math_quotient;


	// Boost low freqs into solvable range
	timer8_prescalar = 0;
	while( (timer8_clock_ratio > 32) && (timer8_prescalar < 6) )
	{ 
		timer8_clock_ratio >>= 2;
		timer8_prescalar += 2;
	}


	// Convert shifts into prescalar value
	if 		(timer8_prescalar == 0)	{timer8_prescalar = 1;}	// 0b00
	else if	(timer8_prescalar == 2)	{timer8_prescalar = 4;}	// 0b01
	else if (timer8_prescalar == 4)	{timer8_prescalar = 16;}	// 0b10
	else if (timer8_prescalar == 6)	{timer8_prescalar = 64;}	// 0b11


	// Resulting clock ratio is the scalar
	timer8_scalar = timer8_clock_ratio;
}


static void Timer8_Solve_Period(void)
{

	if (!timer8_target_freq) timer8_target_freq = (timer8_pwm_clk >> 4); // Default : Clk / 16

	// Solve for number of targets per unit of pwm clock
	math_dividend = timer8_pwm_clk;
	math_divisor  = timer8_target_freq;
	eword_divide();
	timer8_clock_ratio = math_quotient;


	// Correction for period mode since output alternates each cycle
	timer8_clock_ratio >>= 1;
	timer8_clock_ratio += 1;


	// Boost low freqs into solvable range
	timer8_prescalar = 0;
	while( (timer8_clock_ratio > 8192) && (timer8_prescalar < 6) )
	{ 
		timer8_clock_ratio >>= 2;
		timer8_prescalar += 2;
	}


	// Convert shifts into prescalar value
	if 		(timer8_prescalar == 0)	{timer8_prescalar = 1;}	// 0b00
	else if	(timer8_prescalar == 2)	{timer8_prescalar = 4;}	// 0b01
	else if (timer8_prescalar == 4)	{timer8_prescalar = 16;}	// 0b10
	else if (timer8_prescalar == 6)	{timer8_prescalar = 64;}	// 0b11


	// Find the closest combination of scalar and count registers through brute force.
	timer8_tolerance = -1;         // Adjust tolerance to solve for prime numbers
	do                      // Tolerance loop
	{
		timer8_scalar = 32;
		timer8_tolerance++;
		do                  // Scalar loop
		{
			math_dividend = timer8_clock_ratio;
			math_divisor = timer8_scalar;
			word_divide();
			timer8_scalar--;

		} while((math_remainder > timer8_tolerance) && (timer8_scalar > 0));
	} while ((math_remainder > timer8_tolerance) && (timer8_tolerance < 32));

	timer8_bound = math_quotient$0;
}

#ENDIF //SOLVER_OPTION

//===================//
// PROGRAM FUNCTIONS //
//===================//


#IF TIMER8_USE_TM2

void Timer2_Stop(void)
{
	if (timer8_tm2_init) $ TIMER8_2_CTL STOP, TIMER8_2_OUT, TIMER8_2_MODE, TIMER8_2_POL;
}


void Timer2_Start(void)
{
	if (timer8_tm2_init) 
	{
		TIMER8_2_CNT = 0;
		$ TIMER8_2_CTL TIMER8_2_CLK, TIMER8_2_OUT, TIMER8_2_MODE, TIMER8_2_POL;
	}
}


void Timer2_Set_Parameters(void)
{
	if (timer8_tm2_init)
	{
		if (timer8_use_solver) 
		{
			timer8_pwm_clk =  TIMER8_2_HZ;
			#IFIDNI TIMER8_2_MODE, PWM
				if (TIMER8_2_6BIT) timer8_use_6b_pwm = 1;
				else timer8_use_6b_pwm = 0;
				Timer8_Solve_PWM();
				Timer8_Solve_Duty();
			#ELSE
				Timer8_Solve_Period();
			#ENDIF	
		}

		TIMER8_2_BND = timer8_bound;

		switch (timer8_prescalar)
		{
			case 64 :   TIMER8_2_SCL = TIMER8_2_RES | 0b01100000 | timer8_scalar;
						break;

			case 16 :   TIMER8_2_SCL = TIMER8_2_RES | 0b0100000 | timer8_scalar;
						break;

			case  4 :   TIMER8_2_SCL = TIMER8_2_RES | 0b0100000 | timer8_scalar;
						break;

			default :   TIMER8_2_SCL = TIMER8_2_RES | timer8_scalar;

		}	
	}
}


void Timer2_Initialize(void)
{
		if (!timer8_tm2_init)
		{
			timer8_tm2_init = 1;
			Timer2_Stop();
		}
}


void Timer2_Release(void)
{
	if (timer8_tm2_init) 
	{
		TIMER8_2_CTL = 0;
		timer8_tm2_init = 0;
	}
}

#ENDIF // TIMER8_USE_TM2

