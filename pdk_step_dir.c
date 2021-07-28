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
#include "pdk_pwm_11b.h"


//===========//
// VARIABLES //
//===========//

BYTE	step_flags = 0;
BIT		step_dir : step_flags.?;		// Motor driver direction flag
BIT		step_dist_mode : step_flags.?;	// Motor mode flag
BIT		step_module_initialized : step_flags.?;

WORD	step_input;						// Motor driver velocity/distance input
EWORD	step_num_steps;					// Distance target, max 0xFFFFFF


//==================//
// STATIC FUNCTIONS //
//=================-//



//===================//
// PROGRAM FUNCTIONS //
//===================//


// NOTE:
//	While 11b PWM should work, it seems like it's a bit overkill for
//  steppers, which typically have sub 1k pulse rates. A better approach
//  might be to make use of the 8b timers.

void	Step_Initialize	(void)
{
	if (! step_module_initialized)
	{
		
	}
}
void	Step_Set_Dir	(void);
void	Step_Set_Vel	(void);
void 	Step_Mode_Cont	(void);	// Continuous motion
void	Step_Mode_Dist	(void);	// Controlled motion
void	Step_Start		(void);
void 	Step_Stop		(void);
void	Step_Release	(void);