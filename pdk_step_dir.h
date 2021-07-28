/* pdk_step_dir.h
   
Stepper driver utility declarations. 
Define PERIPH_STEP_DIR in system_settings.h

ROM Consumed : ?
RAM Consumed : ?


This software is licensed under GPLv3 <http://www.gnu.org/licenses/>.
Any modifications or distributions have to be licensed under GPLv3.
No warranty of any kind and copyright holders cannot be held liable.
Licensees cannot remove copyright notices.

Copyright (c) 2021 Robert R. Puccinelli
*/

EXTERN BIT		step_dir;		// Motor driver direction flag
EXTERN WORD		step_input;		// Motor driver velocity/distance input
EXTERN EWORD	step_num_steps;	// Distance target, max 0xFFFFFF

void	Step_Initialize	(void);
void	Step_Set_Dir	(void);
void	Step_Set_Vel	(void);
void 	Step_Mode_Cont	(void);	// Continuous motion
void	Step_Mode_Dist	(void);	// Controlled motion
void	Step_Start		(void);
void 	Step_Stop		(void);
void	Step_Release	(void);