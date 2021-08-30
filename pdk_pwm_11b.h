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


This software is licensed under GPLv3 <http://www.gnu.org/licenses/>.
Any modifications or distributions have to be licensed under GPLv3.
No warranty of any kind and copyright holders cannot be held liable.
Licensees cannot remove copyright notices.

Copyright (c) 2021 Robert R. Puccinelli
*/

//===========//
// VARIABLES //
//===========//

EXTERN BYTE	pwm11_prescalar;		// 6-bit  [1, 4, 16, 64]
EXTERN WORD	pwm11_scalar;			// 5-bit  [0 : 31]
EXTERN WORD	pwm11_counter;		// 11-bit [0 : 2046] in steps of 2
EXTERN WORD	pwm11_duty;			// 11-bit [0 : 2047]
EXTERN BIT  pwm11_use_solver;


//======================//
// PWM SOLVER VARIABLES //
//======================//
#IF PERIPH_PWM_11B
#IF PWM_SOLVER_ENABLE

EXTERN BYTE  &pwm11_duty_percent;
EXTERN EWORD &pwm11_target_freq;	// Hz

#ENDIF
#ENDIF //PERIPH_PWM_11B

//===================//
// PROGRAM FUNCTIONS //
//===================//

void	PWM11_0_Initialize     (void);
void	PWM11_0_Set_Parameters (void);
void	PWM11_0_Start          (void);
void	PWM11_0_Stop           (void);
void	PWM11_0_Release        (void);

void	PWM11_1_Initialize     (void);
void	PWM11_1_Set_Parameters (void);
void	PWM11_1_Start          (void);
void	PWM11_1_Stop           (void);
void	PWM11_1_Release        (void);

void	PWM11_2_Initialize     (void);
void	PWM11_2_Set_Parameters (void);
void	PWM11_2_Start          (void);
void	PWM11_2_Stop           (void);
void	PWM11_2_Release        (void);
