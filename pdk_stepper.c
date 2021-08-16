/* pdk_step_dir.c
   
Stepper driver utilities. 
Define PERIPH_STEP_DIR in system_settings.h

ROM Consumed : ?
RAM Consumed : ?




This software is licensed under GPLv3 <http://www.gnu.org/licenses/>.
Any modifications or distributions have to be licensed under GPLv3.
No warranty of any kind and copyright holders cannot be held liable.
Licensees cannot remove copyright notices.

Copyright (c) 2021 Robert R. Puccinelli
*/

#include "system_settings.h"
#include "pdk_math.h"
#include "pdk_timer_8b.h"
#include "pdk_pwm_11b.h"


//===========//
// VARIABLES //
//===========//

BYTE       stepper_flags = 0;
BIT        stepper_dir                : stepper_flags.?; // Motor driver direction flag
BIT        stepper_dist_mode          : stepper_flags.?; // Motor mode flag
BIT        stepper_is_moving          : stepper_flags.?; // Flag to report status
STATIC BIT stepper_module_initialized : stepper_flags.?;

EWORD stepper_dist_per_run;  // target distance if in dist_mode
WORD  stepper_units_per_min; // target velocity
WORD  stepper_units_per_rev;
WORD  stepper_steps_per_rev;

STATIC EWORD steps_remaining;

#ifidni STEPPER_TIMER_SRC, PWM0
	STATIC EWORD &target_freq = pwm11_target_freq;
#else
	STATIC EWORD &target_freq = timer8_target_freq;
#endif


//==================//
// STATIC FUNCTIONS //
//==================//


static void Set_Dist(void)
{
	math_mult_a = stepper_units_per_rev;
	math_mult_b = stepper_steps_per_rev;
	word_multiply();

	math_quotient = stepper_dist_per_run;
	math_divisor = math_product;
	steps_remaining = math_dividend;

	if (steps_remaining == 0) steps_remaining = 1;
}


//===================//
// PROGRAM FUNCTIONS //
//===================//

void Stepper_Enable (void)
{
	#if STEPPER_ENABLE_INV
		$ STEPPER_PIN_EN OUT, LOW;
	#else
		$ STEPPER_PIN_EN OUT, HIGH;
	#endif
}


void Stepper_Disable (void)
{
	#if STEPPER_ENABLE_INV
		$ STEPPER_PIN_EN OUT, HIGH;
	#else
		$ STEPPER_PIN_EN OUT, LOW;
	#endif
}


void Stepper_Initialize (void)
{
	if (! stepper_module_initialized)
	{
		$ STEPPER_PIN_DIR  OUT, LOW;
		$ STEPPER_PIN_STEP OUT, LOW;
		stepper_module_initialized = 1;
		Stepper_Disable();
	}
}


void Stepper_Set_Dir (void)
{
	if (stepper_module_initialized) 
	{
		if (stepper_dir) STEPPER_PIN_DIR = 1;
		else STEPPER_PIN_DIR = 0;
	}
}


void Stepper_Set_Vel (void)
{
	if (stepper_module_initialized) 
	{
		math_mult_a = stepper_units_per_min;
		math_mult_b = stepper_steps_per_rev;
		word_multiply();

		math_quotient = math_product;
		math_divisor = 60;
		eword_divide();

		math_quotient = math_dividend;
		math_divisor = stepper_units_per_rev;
		eword_divide();

		target_freq = math_dividend;

		#ifidni STEPPER_TIMER_SRC, PWM0
			pwm11_use_solver = 1;
			pwm11_duty_percent = 50;
			PWM11_0_Set_Parameters();

		#elseifidni STEPPER_TIMER_SRC, TM2
			timer8_use_solver = 1;
			Timer2_Set_Parameters();

		#elseifidni STEPPER_TIMER_SRC, TM3
			timer8_use_solver = 1;
			Timer3_Set_Parameters();

		#endif
	}
}


void Stepper_Start (void)
{
	if (stepper_module_initialized)
	{
		if (stepper_dist_mode)
		{
			Set_Dist();
			INTRQ.STEPPER_INTR = 0;
			INTEN.STEPPER_INTR = 1;
		}
		else INTEN.STEPPER_INTR = 0;

		#ifidni STEPPER_TIMER_SRC, PWM0
			PWM11_0_Start();

		#elseifidni STEPPER_TIMER_SRC, TM2
			Timer2_Start();

		#elseifidni STEPPER_TIMER_SRC, TM3
			Timer3_Start();

		#endif
		stepper_is_moving = 1;
	}
}


void Stepper_Stop (void)
{
	if (stepper_module_initialized)
	{
		#ifidni STEPPER_TIMER_SRC, PWM0
			PWM11_0_Stop();

		#elseifidni STEPPER_TIMER_SRC, TM2
			Timer2_Stop();

		#elseifidni STEPPER_TIMER_SRC, TM3
			Timer3_Stop();

		#endif
		stepper_is_moving = 0;
	}
}


void Stepper_Dist_Mode_Interrupt (void)
{
	steps_remaining--;
	if (!steps_remaining) Stepper_Stop();
	INTRQ.STEPPER_INTR = 0;
}


void Stepper_Release (void)
{
	if (stepper_module_initialized)
	{
		$ STEPPER_PIN_EN   IN;
		$ STEPPER_PIN_DIR  IN;
		$ STEPPER_PIN_STEP IN;
		stepper_module_initialized = 0;
	}
}