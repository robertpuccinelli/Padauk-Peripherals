/* pdk_stepper.h
   
Stepper driver utility declarations. 
Define PERIPH_STEP_DIR in system_settings.h

ROM Consumed : 639B / 0x27F - 8b TIMER
RAM Consumed :  51B / 0x033 - 8b TIMER

ROM Consumed : 728B / 0x2D8 - 11b PWM TIMER
RAM Consumed :  57B / 0x039 - 11b PWM TIMER


This software is licensed under GPLv3 <http://www.gnu.org/licenses/>.
Any modifications or distributions have to be licensed under GPLv3.
No warranty of any kind and copyright holders cannot be held liable.
Licensees cannot remove copyright notices.

Copyright (c) 2021 Robert R. Puccinelli
*/

//===========//
// VARIABLES //
//===========//

EXTERN BIT   stepper_dir;           // Motor driver direction flag
EXTERN BIT   stepper_dist_mode;     // Motor mode flag
EXTERN BIT   stepper_is_moving;     // Motor status flag
EXTERN BIT   stepper_enabled;       // Read-only enabled flag
EXTERN EWORD stepper_units_per_run;  // Distance to travel in dist mode
EXTERN WORD  stepper_units_per_min; // Velocity
EXTERN WORD  stepper_units_per_rev; // Must match system configuration
EXTERN WORD  stepper_steps_per_rev; // Must match driver configuration


//===================//
// PROGRAM FUNCTIONS //
//===================//

void	Stepper_Initialize          (void);
void	Stepper_Enable              (void);
void	Stepper_Disable             (void);
void	Stepper_Set_Dir             (void);
void	Stepper_Set_Vel             (void);
void	Stepper_Dist_Mode_Interrupt (void);
void	Stepper_Start               (void);
void 	Stepper_Stop                (void);
void	Stepper_Release	            (void);