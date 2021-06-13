/* pdk_pwm_11b.h
   
11-bit PWM declarations for ICs with 11-bit PWM functionality.
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

//===========//
// VARIABLES //
//===========//

BYTE	prescaler;		// 6-bit  [1, 4, 16, 64]
WORD	scalar;			// 5-bit  [0 : 31]
WORD	counter;		// 11-bit [0 : 2046] in steps of 2


//======================//
// PWM SOLVER VARIABLES //
//======================//

#IF SOLVER_OPTION

DWORD	f_pwm_target;	// Hz
BIT		use_pwm_solver; // Flag to select PWM solver, if available

#ENDIF


//===================//
// PROGRAM FUNCTIONS //
//===================//

void	PWM_11b_Initialize (void);
void	PWM_11b_Set_Parameters(void);
void	PWM_11b_Start (void);
void	PWM_11b_Stop (void);
void	PWM_11b_Release (void);
