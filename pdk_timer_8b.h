/* pdk_timer_8b.h
   
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

//===========//
// VARIABLES //
//===========//

EXTERN BYTE active_timer;	// Timer number to use, ie 2 or 3
EXTERN BYTE	prescaler;		// 6-bit  [1, 4, 16, 64]
EXTERN WORD	scalar;			// 5-bit  [0 : 31]
EXTERN WORD	bound;			// 8-bit bound register


//======================//
// PWM SOLVER VARIABLES //
//======================//

#IF SOLVER_OPTION

EXTERN DWORD	f_pwm_target;	// Hz
EXTERN BIT		use_pwm_solver; // Flag to select PWM solver, if available

#ENDIF


//===================//
// PROGRAM FUNCTIONS //
//===================//

void    Timer_Select(void);
void	Timer_Initialize (void);
void	Timer_Set_Parameters(void);
void	Timer_Start (void);
void	Timer_Stop (void);
void	Timer_Release (void);
