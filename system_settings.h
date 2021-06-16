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
#define PERIPH_BUTTON	1		//	Buttons.	Disable: 0, Enable: 1


//======================//
// RESOURCE UTILIZATION //
//======================//
//
// Use this area to track which resources are used and where
//
// 	PA0	-				PB0	-				PC0	X
// 	PA1	X				PB1	-				PC1	X	
// 	PA2	X				PB2	BTN				PC2	X
// 	PA3	STEP_D			PB3	-				PC3	X
// 	PA4	STEP_S			PB4	BTN				PC4	X
// 	PA5	-				PB5	BTN				PC5	X
// 	PA6	I2C_SDA			PB6	BTN				PC6	X
//	PA7	I2C_SCL			PB7	BTN				PC7	X
//
//	TM16	-
//	TM2		BTN
//	TM3		-
//
//	PWMG0	STEP_S
//	PWMG1	-
//	PWMG2	-
//
//	ADC		-
//	COMP	-
//	LCD		X
//	OPA		X
//	TOUCH	X
//


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
	#define ILRC_HZ			59000	// Varies with voltage + temperature
#endif


#ifidni IC_TARGET, PMS132
	#define HAS_MULTIPLIER	1
	#define HAS_PWM			1
	#define HAS_11B_PWM		1
	#define HAS_CMP			1
	#define HAS_ADC			1
	#define HAS_12B_ADC		1
	#define HAS_OPA			0
	#define ILRC_HZ			53000	// Varies with voltage + temperature
#endif


#ifidni IC_TARGET, PMS134
	#define HAS_MULTIPLIER	1
	#define HAS_PWM			1
	#define HAS_11B_PWM		1
	#define HAS_CMP			1
	#define HAS_ADC			1
	#define HAS_12B_ADC		1
	#define HAS_OPA			0
	#define ILRC_HZ			53000	// Varies with voltage + temperature
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
	#define PWM_OUTPUT		PB5			// Disable, PB5, PA0, PB4
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


//==============//
// BUTTON INPUT //
//==============//
#ifidni PERIPH_BUTTON, 1
	#define BTN_WAKE_SYS	1

	// Debouncer timer selection
	#define BTN_TIMER		TM2
	#define BTN_TIMER_CTL	TM2C
	#define BTN_TIMER_CNT	TM2CT
	#define BTN_TIMER_BND	TM2B
	#define BTN_TIMER_SCL	TM2S
	#define BTN_TIMER_CLK	ILRC		// Refer to datasheet and XXX.INC for options


	// Debouncer timing settings
	#define BTN_DEBOUNCE_T	5			// ms to wait before validating button state	
	#define BTN_TIMER_FREQ 	ILRC_HZ		// ILRC_HZ is defined in IC definition above
	#define BTN_TIMER_DIV	31			// [0 : 31] Clock divider
	// 	# of timer cycles = (timer Hz / (clock divider + 1)) x (wait in ms / 1000);
	//  # of timer cycles MUST BE [1:255]
	//
	// 	Example:
	//		# cycles  =  (60000 / 31 + 1) x (5 / 1000)  =  9 cycles


	// REVIEW IC DATASHEET FOR PIN COMPATABILITY!
	#define BTN_USE_PA	0		// Options: Disable bank: 0 / Enable bank: 1
	#define BTN_PA0		0		// Options: 0 / 1
	#define BTN_PA1		0		// Options: 0 / 1
	#define BTN_PA2		0		// Options: 0 / 1
	#define BTN_PA3		0		// Options: 0 / 1
	#define BTN_PA4		0		// Options: 0 / 1
	#define BTN_PA5		0		// Options: 0 / 1
	#define BTN_PA6		0		// Options: 0 / 1
	#define BTN_PA7		0		// Options: 0 / 1

	#define BTN_USE_PB	1		// Options: Disable bank: 0 / Enable bank: 1
	#define BTN_PB0		0		// Options: 0 / 1
	#define BTN_PB1		1		// Options: 0 / 1
	#define BTN_PB2		1		// Options: 0 / 1
	#define BTN_PB3		1		// Options: 0 / 1
	#define BTN_PB4		1		// Options: 0 / 1
	#define BTN_PB5		1		// Options: 0 / 1
	#define BTN_PB6		1		// Options: 0 / 1
	#define BTN_PB7		1		// Options: 0 / 1

	#define BTN_USE_PC	0		// Options: Disable bank: 0 / Enable bank: 1
	#define BTN_PC0		0		// Options: 0 / 1
	#define BTN_PC1		0		// Options: 0 / 1
	#define BTN_PC2		0		// Options: 0 / 1
	#define BTN_PC3		0		// Options: 0 / 1
	#define BTN_PC4		0		// Options: 0 / 1
	#define BTN_PC5		0		// Options: 0 / 1
	#define BTN_PC6		0		// Options: 0 / 1
	#define BTN_PC7		0		// Options: 0 / 1


	// DO NOT TOUCH -- START //
	#if BTN_USE_PA
		#define BTN_PA	((BTN_PA7 << 7) | \
						 (BTN_PA6 << 6) | \
						 (BTN_PA5 << 5) | \
						 (BTN_PA4 << 4) | \
						 (BTN_PA3 << 3) | \
						 (BTN_PA2 << 2) | \
						 (BTN_PA1 << 1) | \
						 (BTN_PA0 << 0))

		#define BTN_NUM_CB_A 	BTN_PA7 + BTN_PA6 + BTN_PA5 + BTN_PA4 +\
								BTN_PA3 + BTN_PA2 + BTN_PA1 + BTN_PA0
	#endif
	#if BTN_USE_PB
		#define BTN_PB	((BTN_PB7 << 7) | \
						 (BTN_PB6 << 6) | \
						 (BTN_PB5 << 5) | \
						 (BTN_PB4 << 4) | \
						 (BTN_PB3 << 3) | \
						 (BTN_PB2 << 2) | \
						 (BTN_PB1 << 1) | \
						 (BTN_PB0 << 0))

		#define BTN_NUM_CB_B 	BTN_PB7 + BTN_PB6 + BTN_PB5 + BTN_PB4 +\
								BTN_PB3 + BTN_PB2 + BTN_PB1 + BTN_PB0
	#endif
	#if BTN_USE_PC
		#define BTN_PC	((BTN_PC7 << 7) | \
						 (BTN_PC6 << 6) | \
						 (BTN_PC5 << 5) | \
						 (BTN_PC4 << 4) | \
						 (BTN_PC3 << 3) | \
						 (BTN_PC2 << 2) | \
						 (BTN_PC1 << 1) | \
						 (BTN_PC0 << 0))

		#define BTN_NUM_CB_C 	BTN_PC7 + BTN_PC6 + BTN_PC5 + BTN_PC4 +\
								BTN_PC3 + BTN_PC2 + BTN_PC1 + BTN_PC0
	#endif
	// DO NOT TOUCH -- END //

#endif	