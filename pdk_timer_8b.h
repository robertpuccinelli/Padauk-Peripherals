/* pdk_timer_8b.h
   
8-bit timer utilities.


ROM Consumed : 132B / 0x84  -  WITHOUT SOLVER & TM2/TM3
RAM Consumed : 12B  / 0x0C  -  WITHOUT SOLVER & TM2/TM3

ROM Consumed : 595B / 0x253 -  WITH SOLVER & TM2/TM3
RAM Consumed :  35B / 0x23  -  WITH SOLVER & TM2/TM3


NOTE:
	Approximately 1/2 of the solver memory is due to the math library.

	Period solver takes ~2.5ms to complete. Nearly all solutions <10ms.
	PWM solver takes ~500us to complete.

	
DOCUMENTATION ERROR:

	The PMS132 datasheet claims that the bound register for a 8b timer
	can range from 0 - 255. That was not observed to be the case for
	a timer in period mode. When bound = 0 in period mode, the output
	will not change states. PWM mode was not checked. To offset this
	discovery, autosolver iterates through bound instead of scalar.
	This makes it more likely that bound will not be 0.

	
This software is licensed under GPLv3 <http://www.gnu.org/licenses/>.
Any modifications or distributions have to be licensed under GPLv3.
No warranty of any kind and copyright holders cannot be held liable.
Licensees cannot remove copyright notices.

Copyright (c) 2021 Robert R. Puccinelli
*/

//===========//
// VARIABLES //
//===========//

EXTERN BYTE	timer8_prescaler;  // 6-bit  [1, 4, 16, 64]
EXTERN WORD	timer8_scalar;     // 5-bit  [0 : 31]
EXTERN WORD	timer8_bound;      // 8-bit bound register


// SOLVER VARIABLES - ONLY AVAILABLE WHEN TIMER8_SOLVER_ENABLE IS SET TO 1
EXTERN BIT    timer8_use_solver;   // Flag to select parameter solver
EXTERN BYTE  &timer8_duty_percent; // Duty [0 : 100]
EXTERN EWORD &timer8_target_freq;  // Target PWM Hz


//===================//
// PROGRAM FUNCTIONS //
//===================//

void	Timer2_Initialize     (void);
void	Timer2_Set_Parameters (void);
void	Timer2_Start          (void);
void	Timer2_Stop           (void);
void	Timer2_Release        (void);

void	Timer3_Initialize     (void);
void	Timer3_Set_Parameters (void);
void	Timer3_Start          (void);
void	Timer3_Stop           (void);
void	Timer3_Release        (void);