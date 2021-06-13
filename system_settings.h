/* system_settings.h

This file is used to configure peripherals on the target Padauk IC.

Include this file as well as the desired peripheral source and header files
into your Padauk IDE project. To configure:
	1. Change SYSTEM_CLOCK to match SYSCLK defined in your project
	2. Change IC_TARGET to match the Padauk microcontroller being programmed
		NOTE: 	If your IC is not listed, create an entry in the
				SUPPORTED MICROCONTROLLERs section. It is important
				to correctly define the IC features. Refer to datasheet.
	3. Enable the desired pheripherals such as PERIPH_I2C
	4. Change peripheral configuration in the PERIPHERAL SETTINGS section

ROM Used : 0B
RAM Used : 0B


This software is licensed under GPLv3 <http://www.gnu.org/licenses/>.
Any modifications or distributions have to be licensed under GPLv3.
No warranty of any kind and copyright holders cannot be held liable.
Licensees cannot remove copyright notices.

Copyright (c) 2021 Robert R. Puccinelli
*/


#define System_Clock	4000000 // 	Change to match your choice of SYSCLK in Hz
#define IC_TARGET		PMS132	//  PMS150C, PMS132, PMS134
#define PERIPH_I2C_M	1		// 	I2C Master. Disable: 0, Enable: 1
#define PERIPH_PWM_11B	1		// 	11B PWM.    Disable: 0, Enable: 1


//============================//
// SUPPORTED MICROCONTROLLERS //
//============================//


#ifidni IC_TARGET, PMS150C
	#define HAS_MULTIPLIER	0
	#define HAS_PWM			1
	#define HAS_11B_PWM		0
	#define HAS_CMP			1
	#define HAS_ADC			0
	#define HAS_12B_ADC		0
	#define HAS_OPA			0
#endif


#ifidni IC_TARGET, PMS132
	#define HAS_MULTIPLIER	1
	#define HAS_PWM			1
	#define HAS_11B_PWM		1
	#define HAS_CMP			1
	#define HAS_ADC			1
	#define HAS_12B_ADC		1
	#define HAS_OPA			0
#endif


#ifidni IC_TARGET, PMS134
	#define HAS_MULTIPLIER	1
	#define HAS_PWM			1
	#define HAS_11B_PWM		1
	#define HAS_CMP			1
	#define HAS_ADC			1
	#define HAS_12B_ADC		1
	#define HAS_OPA			0
#endif


//=====================//
// PERIPHERAL SETTINGS //
//=====================//


//============//
// I2C MASTER //
//============//
#ifidni PERIPH_I2C_M, 1
	#define I2C_SDA		PA.6
	#define I2C_SCL		PA.7
	#define	FPPA_Duty	16		//	/2, /4, /8, /16

	//		T_xxx	nS
	#define T_High	4700
	#define T_Low	4700
	#define T_Start	4700
	#define T_Stop	4700
	#define T_Buf	4700

	// Addresses
	#define	ST7032		0b0111110	// LCD Controller
	#define	MC24LC00T	0b1010000	// EEPROM, Microchip

#endif


//=========//
// 11b PWM //
//=========//
#ifidni PERIPH_PWM_11B, 1
	#define PWM_CLOCK_HZ	System_Clock // Or IHRC / IHRC x2 in Hz
	#define PWM_CTL			PWMG0C		// 0, 1, 2
	#define PWM_SCALAR		PWMG0S
	#define PWM_COUNT_H		PWMG0CUBH
	#define PWM_COUNT_L		PWMG0CUBL
	#define PWM_DUTY_H		PWMG0DTH
	#define PWM_DUTY_L		PWMG0DTL

	#define PWM_CLOCK		SYSCLK		// SYSCLK, IHRC, IHRC*2
	#define PWM_OUTPUT		PB5			// Disable, PB5, X, PA0, PB4
	#define PWM_VAL_DUTY_L	0b000
	#define PWM_VAL_DUTY_H	0b10000000
	#define PWM_LOAD_SOLVER	1			// Compile PWM solver, if supported.  
										// CHECK HEADER FOR RESOURCE USAGE!!
	// DO NOT TOUCH -- START //
	#if HAS_MULTIPLIER
		#define SOLVER_OPTION PWM_LOAD_SOLVER
	#else
		#define SOLVER_OPTION 0
	#endif
	// DO NOT TOUCH -- END //

#endif
