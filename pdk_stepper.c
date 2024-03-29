/* pdk_stepper.h
   
Stepper driver utility declarations. 
Define PERIPH_STEP_DIR in system_settings.h

ROM Consumed : 639B / 0x27F - 8b TIMER
RAM Consumed :  53B / 0x035 - 8b TIMER

ROM Consumed : 728B / 0x2D8 - 11b PWM TIMER
RAM Consumed :  59B / 0x03B - 11b PWM TIMER


This software is licensed under GPLv3 <http://www.gnu.org/licenses/>.
Any modifications or distributions have to be licensed under GPLv3.
No warranty of any kind and copyright holders cannot be held liable.
Licensees cannot remove copyright notices.

Copyright (c) 2021 Robert R. Puccinelli
*/

#include "system_settings.h"

#IF PERIPH_STEPPER
#include "pdk_math.h"
#include "pdk_timer_8b.h"
#include "pdk_pwm_11b.h"


//===========//
// VARIABLES //
//===========//

BYTE       stepper_flags = 0;
BIT        stepper_dir                : stepper_flags.?; // Motor driver direction flag
BIT        stepper_dist_mode          : stepper_flags.?; // Motor mode flag
BIT        stepper_is_moving          : stepper_flags.?; // Flag to report moving status
BIT        stepper_enabled            : stepper_flags.?; // Flag to report enabled status
STATIC BIT stepper_module_initialized : stepper_flags.?;
STATIC BIT stepper_timer_period_state : stepper_flags.?;

EWORD stepper_units_per_run; // target distance if in dist_mode
WORD  stepper_units_per_min; // target velocity
WORD  stepper_units_per_rev;
WORD  stepper_steps_per_rev;

STATIC WORD revs_remaining    = 0;
STATIC WORD final_step_target = 0;
STATIC WORD current_step      = 0;

#IFIDNI STEPPER_TIMER_SRC, PWM0
	#DEFINE target_freq pwm11_target_freq
#ELSE
	#DEFINE target_freq timer8_target_freq
#ENDIF


//==================//
// STATIC FUNCTIONS //
//==================//


static void Set_Dist(void)
{
	// Need to convert dist tracking to revs. Cannot multiply dist by steps per rev. Limits dist to WORD.
	math_dividend = stepper_units_per_run;
	math_divisor  = stepper_units_per_rev;
	dword_divide();

	revs_remaining = math_quotient;

	math_mult_a = math_remainder;
	math_mult_b = stepper_steps_per_rev;
	word_multiply();

	math_dividend = math_product;
	math_divisor = stepper_units_per_rev;
	dword_divide();

	final_step_target = math_quotient;

	stepper_timer_period_state = 0;
	if (!revs_remaining && final_step_target == 0) final_step_target = 1;
}


//===================//
// PROGRAM FUNCTIONS //
//===================//

void Stepper_Enable (void)
{
	#IF STEPPER_ENABLE_INV
		$ PA.3 OUT, LOW;
	#ELSE
		$ STEPPER_PIN_EN OUT, HIGH;
	#ENDIF
	stepper_enabled = 1;
}


void Stepper_Disable (void)
{
	#IF STEPPER_ENABLE_INV
		$ PA.3 OUT, HIGH;
	#ELSE
		$ STEPPER_PIN_EN OUT, LOW;
	#ENDIF
	stepper_enabled = 0;
}


void Stepper_Initialize (void)
{
	if (! stepper_module_initialized)
	{
		$ STEPPER_PIN_DIR  OUT, LOW;
		$ STEPPER_PIN_STEP OUT, LOW;
		stepper_module_initialized = 1;
		Stepper_Disable();

		#IFIDNI STEPPER_TIMER_SRC, PWM0
			PWM11_0_Initialize();
		#ELSEIFIDNI STEPPER_TIMER_SRC, TM2
			Timer2_Initialize();
		#ELSEIFIDNI STEPPER_TIMER_SRC, TM3
			Timer3_Initialize();
		#ENDIF
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
		// WARNING
		//
		// units/min x steps/rev / ( 60 x units/rev) must be < 0xFFFFFF
		math_mult_a = stepper_units_per_min;
		math_mult_b = stepper_steps_per_rev;
		word_multiply();

		math_divisor = 60;
		math_dividend = math_product;
		dword_divide();

		math_dividend = math_quotient;
		math_divisor = stepper_units_per_rev;
		dword_divide();

		target_freq = math_quotient;

		if (!target_freq) target_freq = 1;

		#IFIDNI STEPPER_TIMER_SRC, PWM0
			pwm11_use_solver = 1;
			pwm11_duty_percent = 50;
			PWM11_0_Set_Parameters();
		#ELSEIFIDNI STEPPER_TIMER_SRC, TM2
			timer8_use_solver = 1;
			Timer2_Set_Parameters();
		#ELSEIFIDNI STEPPER_TIMER_SRC, TM3
			timer8_use_solver = 1;
			Timer3_Set_Parameters();
		#ENDIF
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

		#IFIDNI STEPPER_TIMER_SRC, PWM0
			PWM11_0_Start();
		#ELSEIFIDNI STEPPER_TIMER_SRC, TM2
			Timer2_Start();
		#ELSEIFIDNI STEPPER_TIMER_SRC, TM3
			Timer3_Start();
		#ENDIF

		stepper_is_moving = 1;
	}
}


void Stepper_Stop (void)
{
	if (stepper_module_initialized)
	{
		#IFIDNI STEPPER_TIMER_SRC, PWM0
			PWM11_0_Stop();
		#ELSEIFIDNI STEPPER_TIMER_SRC, TM2
			Timer2_Stop();
		#ELSEIFIDNI STEPPER_TIMER_SRC, TM3
			Timer3_Stop();
		#ENDIF

		stepper_is_moving = 0;
	}
}


void Stepper_Dist_Mode_Interrupt (void)
{
	#IFDIFI STEPPER_TIMER_SRC, PWM0
		if (stepper_timer_period_state) 
		{
			current_step++;
			if (current_step == stepper_steps_per_rev) 
			{ 
				revs_remaining--;
				current_step = 0 ;
			}
			if (!revs_remaining && current_step == final_step_target) Stepper_Stop();
			stepper_timer_period_state = 0;
		}
		else stepper_timer_period_state = 1;
	#ELSE
		current_step++;
		if (current_step == stepper_steps_per_rev) 
		{ 
			revs_remaining--;
			current_step = 0 ;
		}
		if (!revs_remaining && current_step == final_step_target) Stepper_Stop();
	#ENDIF

	INTRQ.STEPPER_INTR = 0;
}


void Stepper_Release (void)
{
	if (stepper_module_initialized)
	{
		#IFIDNI STEPPER_TIMER_SRC, PWM0
			PWM11_0_Release();
		#ELSEIFIDNI STEPPER_TIMER_SRC, TM2
			Timer2_Release();
		#ELSEIFIDNI STEPPER_TIMER_SRC, TM3
			Timer3_Release();
		#ENDIF

		$ PA.3   IN;
		$ STEPPER_PIN_DIR  IN;
		$ STEPPER_PIN_STEP IN;
		stepper_module_initialized = 0;
	}
}

#ENDIF //PERIPH_STEPPER
